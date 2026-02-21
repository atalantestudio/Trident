// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/BarrierAccess.hpp"
#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceState.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DispatchIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndirectCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	DX12::CommandEncoder::CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor) :
		Trident::CommandEncoder(device),
		usage(descriptor.usage)
	{
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		ADAPTER_ASSERT(descriptor.usage != CommandBufferUsage::SIMULTANEOUS_USE, "SIMULTANEOUS_USE is not supported on command lists.");

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const DX12::CommandPool* _commandPool = static_cast<const DX12::CommandPool*>(commandPool);
		HRESULT result;

		commandAllocator = _commandPool->commandAllocator;

		result = _device->device->CreateCommandList(0, _commandPool->type, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command list.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = commandList->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set command list name.");

				free(name);
			}
		#endif

		// Only 1 open command encoder at a time.
		if (descriptor.usage == CommandBufferUsage::AUTO) {
			end();
		}
	}

	void DX12::CommandEncoder::begin() const {}

	void DX12::CommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		framebuffer = std::static_pointer_cast<DX12::Framebuffer>(descriptor.framebuffer);

		const ref<RenderPass> renderPass = framebuffer->getRenderPass();

		UINT attachmentIndex = 0;

		D3D12_RECT rectangle{};
		rectangle.left = 0;
		rectangle.top = 0;
		rectangle.right = framebuffer->getExtent().width;
		rectangle.bottom = framebuffer->getExtent().height;

		// TODO: Use sequence<D3D12_RESOURCE_BARRIER>.
		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[attachmentIndex]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (image->resourceState == D3D12_RESOURCE_STATE_RENDER_TARGET || image->resourceState == D3D12_RESOURCE_STATE_DEPTH_WRITE) {
				attachmentIndex += 1;

				continue;
			}

			D3D12_RESOURCE_BARRIER resourceBarrier{};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = image->resource;
			resourceBarrier.Transition.Subresource = imageView->getLayerOffset();
			resourceBarrier.Transition.StateBefore = image->resourceState;

			if (isDepthFormat(image->getFormat())) {
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			}
			else {
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			}

			image->resourceState = resourceBarrier.Transition.StateAfter;

			resourceBarriers.emplace_back(resourceBarrier);

			attachmentIndex += 1;
		}

		if (resourceBarriers.size() > 0) {
			commandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
		}

		attachmentIndex = 0;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

			if (attachment.loadOperation != LoadOperation::CLEAR) {
				attachmentIndex += 1;

				continue;
			}

			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[attachmentIndex]);
			const ClearValue& clearValue = descriptor.clearValues[attachmentIndex];

			if (isDepthFormat(imageView->getImage()->getFormat())) {
				commandList->ClearDepthStencilView(imageView->descriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearValue.depth, static_cast<UINT8>(clearValue.stencil), 1, &rectangle);
			}
			else {
				commandList->ClearRenderTargetView(imageView->descriptorHandle, clearValue.color, 1, &rectangle);
			}

			attachmentIndex += 1;
		}

		setRenderTargets();

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindComputePipeline(ref<Trident::ComputePipeline> computePipeline) {
		const ref<DX12::ComputePipeline> pipeline = std::static_pointer_cast<DX12::ComputePipeline>(computePipeline);

		commandList->SetPipelineState(pipeline->pipelineState);
		commandList->SetComputeRootSignature(pipeline->pipelineLayout->rootSignature);

		computePipelineBound = true;
		commandCount += 1;
	}

	void DX12::CommandEncoder::bindRenderPipeline(ref<Trident::RenderPipeline> renderPipeline) {
		const ref<DX12::RenderPipeline> pipeline = std::static_pointer_cast<DX12::RenderPipeline>(renderPipeline);

		commandList->SetPipelineState(pipeline->pipelineState);
		commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature);
		commandList->IASetPrimitiveTopology(pipeline->primitiveTopology);

		samplerDescriptorTableRootParameterIndex = pipeline->pipelineLayout->samplerDescriptorTableRootParameterIndex;

		computePipelineBound = false;
		commandCount += 1;
	}

	void DX12::CommandEncoder::bindDescriptorSets(ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) {
		const ref<DX12::MemoryAllocator> memoryAllocator = std::static_pointer_cast<DX12::MemoryAllocator>(device->getMemoryAllocator());

		ID3D12DescriptorHeap* viewDescriptorHeap = memoryAllocator->getCurrentViewDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* samplerDescriptorHeap = memoryAllocator->getCurrentSamplerDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* descriptorHeaps[2] = {viewDescriptorHeap, samplerDescriptorHeap};

		commandList->SetDescriptorHeaps(2, &descriptorHeaps[0]);

		UINT rootParameterIndex = 0;

		if (computePipelineBound) {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * memoryAllocator->viewDescriptorHandleIncrementSize;

				commandList->SetComputeRootDescriptorTable(descriptorSetIndexOffset + rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (samplerDescriptorTableRootParameterIndex != max<UINT>()) {
				UINT64 samplerDescriptorHandleOffset = 0;

				rootParameterIndex = 0;

				while (rootParameterIndex < descriptorSets.count()) {
					const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

					samplerDescriptorHandleOffset = max(samplerDescriptorHandleOffset, descriptorSet->samplerDescriptorHandleOffset);
					rootParameterIndex += 1;
				}

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += samplerDescriptorHandleOffset * memoryAllocator->samplerDescriptorHandleIncrementSize;

				commandList->SetComputeRootDescriptorTable(samplerDescriptorTableRootParameterIndex, descriptorOffset);
			}
		}
		else {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * memoryAllocator->viewDescriptorHandleIncrementSize;

				commandList->SetGraphicsRootDescriptorTable(descriptorSetIndexOffset + rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (samplerDescriptorTableRootParameterIndex != max<UINT>()) {
				UINT64 samplerDescriptorHandleOffset = 0;

				rootParameterIndex = 0;

				while (rootParameterIndex < descriptorSets.count()) {
					const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

					samplerDescriptorHandleOffset = max(samplerDescriptorHandleOffset, descriptorSet->samplerDescriptorHandleOffset);
					rootParameterIndex += 1;
				}

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += samplerDescriptorHandleOffset * memoryAllocator->samplerDescriptorHandleIncrementSize;

				commandList->SetGraphicsRootDescriptorTable(samplerDescriptorTableRootParameterIndex, descriptorOffset);
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		ADAPTER_ASSERT(offset < vertexBuffer->getSize(), "The vertex buffer offset is overflowing.");

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(vertexBuffer)->resource->GetGPUVirtualAddress() + offset;
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer->getSize() - offset);
		vertexBufferView.StrideInBytes = stride;

		commandList->IASetVertexBuffers(bindingIndex, 1, &vertexBufferView);

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToBuffer(ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		const ref<DX12::Buffer> _destinationBuffer = std::static_pointer_cast<DX12::Buffer>(destinationBuffer);

		// NOTE
		// D3D12_HEAP_TYPE_UPLOAD heaps only allow CPU writes.
		// The source and destination buffers are mapped to emulate a GPU copy command.
		if (_destinationBuffer->heapType == D3D12_HEAP_TYPE_UPLOAD) {
			const uint8* sourceMappedRange = static_cast<const uint8*>(sourceBuffer->map(0, sourceBuffer->getSize()));
			uint8* destinationMappedRange = static_cast<uint8*>(destinationBuffer->map(0, destinationBuffer->getSize()));

			for (const BufferCopyRegion& copyRegion : copyRegions) {
				std::copy_n(sourceMappedRange + copyRegion.sourceOffset, copyRegion.size, destinationMappedRange + copyRegion.destinationOffset);
			}

			sourceBuffer->unmap();
			destinationBuffer->unmap();

			commandCount += 1;

			return;
		}

		const ref<DX12::Buffer> _sourceBuffer = std::static_pointer_cast<DX12::Buffer>(sourceBuffer);

		for (const BufferCopyRegion& copyRegion : copyRegions) {
			commandList->CopyBufferRegion(_destinationBuffer->resource, copyRegion.destinationOffset, _sourceBuffer->resource, copyRegion.sourceOffset, copyRegion.size);
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToImage(ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		const ref<DX12::Buffer> srcBuffer = std::static_pointer_cast<DX12::Buffer>(sourceBuffer);
		const ref<DX12::Image> dstImage = std::static_pointer_cast<DX12::Image>(destinationImage);
		const D3D12_RESOURCE_DESC srcBufferResourceDescriptor = srcBuffer->resource->GetDesc();
		const D3D12_RESOURCE_DESC dstImageResourceDescriptor = dstImage->resource->GetDesc();
		const UINT channelCount = getChannelCount(dstImage->getFormat());

		D3D12_TEXTURE_COPY_LOCATION bufferCopyLocation{};
		bufferCopyLocation.pResource = srcBuffer->resource;
		bufferCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		bufferCopyLocation.PlacedFootprint.Footprint.Format = dstImageResourceDescriptor.Format;

		D3D12_TEXTURE_COPY_LOCATION textureCopyLocation{};
		textureCopyLocation.pResource = dstImage->resource;
		textureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			const UINT64 layerSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * channelCount;

			bufferCopyLocation.PlacedFootprint.Footprint.Width = copyRegion.imageExtent.width;
			bufferCopyLocation.PlacedFootprint.Footprint.Height = copyRegion.imageExtent.height;
			bufferCopyLocation.PlacedFootprint.Footprint.Depth = copyRegion.imageExtent.depth;
			bufferCopyLocation.PlacedFootprint.Footprint.RowPitch = copyRegion.imageExtent.width * channelCount;

			UINT layerIndex = 0;

			while (layerIndex < copyRegion.imageLayerCount) {
				bufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset + layerSize * layerIndex;

				textureCopyLocation.SubresourceIndex = copyRegion.imageLayerOffset + layerIndex;

				commandList->CopyTextureRegion(&textureCopyLocation, copyRegion.imageOffsetX, copyRegion.imageOffsetY, copyRegion.imageOffsetZ, &bufferCopyLocation, nullptr);

				layerIndex += 1;
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyImageToBuffer(ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		commandList->Dispatch(groupCountX, groupCountY, groupCountZ);

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatchIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset) {
		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DispatchIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = 1;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			const HRESULT result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			1,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		commandCount += 1;
	}

	void DX12::CommandEncoder::nextSubpass() {
		subpassIndex += 1;

		setRenderTargets();

		commandCount += 1;
	}

	void DX12::CommandEncoder::insertBufferMemoryBarriers(view<BufferMemoryBarrier> barriers) {
		sequence<D3D12_BUFFER_BARRIER> bufferBarriers(barriers.count());

		UINT bufferBarrierCount = 0;

		while (bufferBarrierCount < barriers.count()) {
			const BufferMemoryBarrier& bufferMemoryBarrier = barriers[bufferBarrierCount];

			D3D12_BUFFER_BARRIER& bufferBarrier = bufferBarriers[bufferBarrierCount];
			bufferBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
			bufferBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
			bufferBarrier.AccessBefore = getBarrierAccess(bufferMemoryBarrier.sourceAccess);
			bufferBarrier.AccessAfter = getBarrierAccess(bufferMemoryBarrier.destinationAccess);
			bufferBarrier.pResource = std::static_pointer_cast<DX12::Buffer>(bufferMemoryBarrier.buffer)->resource;
			bufferBarrier.Offset = bufferMemoryBarrier.offset;
			bufferBarrier.Size = bufferMemoryBarrier.size;

			bufferBarrierCount += 1;
		}

		D3D12_BARRIER_GROUP barrierGroup{};
		barrierGroup.Type = D3D12_BARRIER_TYPE_BUFFER;
		barrierGroup.NumBarriers = bufferBarrierCount;
		barrierGroup.pBufferBarriers = &bufferBarriers[0];

		commandList->Barrier(1, &barrierGroup);

		commandCount += 1;
	}

	void DX12::CommandEncoder::setImageMemoryBarrier(ref<Trident::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		const ref<DX12::Image> _image = std::static_pointer_cast<DX12::Image>(image);
		D3D12_RESOURCE_STATES newResourceState = getImageTransitionResourceState(newLayout);

		if (_image->resourceState == newResourceState) {
			ADAPTER_WARNING("Ignored transition resource barrier to the same layout.");

			return;
		}

		if (computePipelineBound && newLayout == ImageLayout::SHADER_READ_ONLY_OPTIMAL) {
			newResourceState = D3D12_RESOURCE_STATE_COMMON;
		}

		D3D12_RESOURCE_BARRIER resourceBarrier{};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = _image->resource;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		resourceBarrier.Transition.StateBefore = _image->resourceState;
		resourceBarrier.Transition.StateAfter = newResourceState;

		_image->resourceState = resourceBarrier.Transition.StateAfter;

		commandList->ResourceBarrier(1, &resourceBarrier);

		image->setLayout(newLayout);

		commandCount += 1;
	}

	void DX12::CommandEncoder::setLineWidth(float32 lineWidth) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		D3D12_RECT rectangle{};
		rectangle.left = xOffset;
		rectangle.top = yOffset;
		rectangle.right = xOffset + extent.width;
		rectangle.bottom = yOffset + extent.height;

		commandList->RSSetScissorRects(1, &rectangle);

		commandCount += 1;
	}

	void DX12::CommandEncoder::setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		D3D12_VIEWPORT viewport{};
		viewport.TopLeftX = origin[0];

		// TODO: Allow the origin's Y to be flipped, like in Vulkan?
		viewport.TopLeftY = 0.0f;

		viewport.Width = abs(size[0]);
		viewport.Height = abs(size[1]);
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;

		commandList->RSSetViewports(1, &viewport);

		commandCount += 1;
	}

	void DX12::CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		commandList->DrawInstanced(vertexCount, instanceCount, vertexOffset, instanceOffset);

		commandCount += 1;
	}

	void DX12::CommandEncoder::drawIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = drawCount;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			const HRESULT result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			drawCount,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		commandCount += 1;
	}

	void DX12::CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		commandList->DrawIndexedInstanced(indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		commandCount += 1;
	}

	void DX12::CommandEncoder::drawIndexedIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndexedIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = drawCount;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			const HRESULT result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			drawCount,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		commandCount += 1;
	}

	void DX12::CommandEncoder::endRenderPass() {
		if (presentRenderTargetIndex < framebuffer->imageViews.count()) {
			ref<DX12::ImageView> presentImageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[presentRenderTargetIndex]);
			DX12::Image* presentImage = static_cast<DX12::Image*>(presentImageView->getImage());

			if (presentImage->getSampleCount() > 1) {
				const ref<Subpass> subpass = framebuffer->getRenderPass()->getSubpass(subpassIndex);
				const ref<DX12::ImageView> resolveImageView = presentImageView;

				DX12::Image* resolveImage = presentImage;

				presentImageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[subpass->getResolveAttachmentReferenceCount() + (subpass->hasDepthStencilAttachmentReference() ? 1 : 0) + presentRenderTargetIndex]);
				presentImage = static_cast<DX12::Image*>(presentImageView->getImage());

				D3D12_RESOURCE_BARRIER resourceBarriers[2];

				D3D12_RESOURCE_BARRIER& resolveResourceBarrier = resourceBarriers[0];
				resolveResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resolveResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resolveResourceBarrier.Transition.pResource = resolveImage->resource;
				resolveResourceBarrier.Transition.Subresource = resolveImageView->getLayerOffset();
				resolveResourceBarrier.Transition.StateBefore = resolveImage->resourceState;
				resolveResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

				resolveImage->resourceState = resolveResourceBarrier.Transition.StateAfter;

				D3D12_RESOURCE_BARRIER& presentResourceBarrier = resourceBarriers[1];
				presentResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				presentResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				presentResourceBarrier.Transition.pResource = presentImage->resource;
				presentResourceBarrier.Transition.Subresource = presentImageView->getLayerOffset();
				presentResourceBarrier.Transition.StateBefore = presentImage->resourceState;
				presentResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

				presentImage->resourceState = presentResourceBarrier.Transition.StateAfter;

				commandList->ResourceBarrier(2, &resourceBarriers[0]);

				commandList->ResolveSubresource(
					presentImage->resource,
					presentImageView->getLayerOffset(),
					resolveImage->resource,
					resolveImageView->getLayerOffset(),
					getImageFormat(presentImage->getFormat())
				);
			}

			D3D12_RESOURCE_BARRIER resourceBarrier{};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = presentImage->resource;
			resourceBarrier.Transition.Subresource = presentImageView->getLayerOffset();
			resourceBarrier.Transition.StateBefore = presentImage->resourceState;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			presentImage->resourceState = resourceBarrier.Transition.StateAfter;

			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		commandCount += 1;
	}

	ref<Trident::CommandBuffer> DX12::CommandEncoder::end() const {
		const HRESULT result = commandList->Close();

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to close command list.");

		return createRef<DX12::CommandBuffer>(commandList, commandCount);
	}

	void DX12::CommandEncoder::reset() {
		if (commandCount == 0 && usage == CommandBufferUsage::ONE_TIME_SUBMIT) {
			return;
		}

		HRESULT result;

		result = commandAllocator->Reset();

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command allocator.");

		result = commandList->Reset(commandAllocator, nullptr);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command list.");

		commandCount = 0;
		computePipelineBound = false;
		framebuffer = nullptr;
		subpassIndex = 0;
		presentRenderTargetIndex = max<UINT>();
		samplerDescriptorTableRootParameterIndex = max<UINT>();
	}

	void DX12::CommandEncoder::bindIndexBuffer(ref<Trident::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		ADAPTER_ASSERT(offset < indexBuffer->getSize(), "The index buffer offset is overflowing.");

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(indexBuffer)->resource->GetGPUVirtualAddress() + offset;
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer->getSize() - offset);
		indexBufferView.Format = getIndexBufferFormat(indexTypeSize);

		commandList->IASetIndexBuffer(&indexBufferView);

		commandCount += 1;
	}

	void DX12::CommandEncoder::setRenderTargets() {
		presentRenderTargetIndex = max<UINT>();

		const ref<RenderPass> renderPass = framebuffer->getRenderPass();
		const ref<Subpass> subpass = renderPass->getSubpass(subpassIndex);
		const view<AttachmentReference> colorAttachmentReferences = subpass->getColorAttachmentReferences();

		if (subpass->getInputAttachmentReferenceCount() > 0) {
			const view<AttachmentReference> inputAttachmentReferences = subpass->getInputAttachmentReferences();

			sequence<D3D12_RESOURCE_BARRIER> resourceBarriers(subpass->getInputAttachmentReferenceCount());
			UINT inputAttachmentReferenceIndex = 0;

			while (inputAttachmentReferenceIndex < inputAttachmentReferences.count()) {
				const AttachmentReference& inputAttachmentReference = inputAttachmentReferences[inputAttachmentReferenceIndex];

				if (inputAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					ENGINE_ERROR_UNIMPLEMENTED();

					inputAttachmentReferenceIndex += 1;

					continue;
				}

				DX12::Image* image = static_cast<DX12::Image*>(framebuffer->imageViews[inputAttachmentReference.index]->getImage());

				D3D12_RESOURCE_BARRIER& resourceBarrier = resourceBarriers[inputAttachmentReferenceIndex];
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource;
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				resourceBarrier.Transition.StateBefore = image->resourceState;

				// TODO: Don't apply NON_PIXEL_SHADER_RESOURCE if the resource is only visible to fragment shaders.
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

				image->resourceState = resourceBarrier.Transition.StateAfter;

				inputAttachmentReferenceIndex += 1;
			}

			commandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.count()), &resourceBarriers[0]);
		}

		UINT renderTargetCount = 0;

		for (const AttachmentReference& colorAttachmentReference : colorAttachmentReferences) {
			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				continue;
			}

			renderTargetCount += 1;
		}

		sequence<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetViewHandles(renderTargetCount);
		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
		UINT renderTargetIndex = 0;
		UINT colorAttachmentIndex = 0;

		while (renderTargetIndex < renderTargetCount) {
			const AttachmentReference& colorAttachmentReference = colorAttachmentReferences[renderTargetIndex];

			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				ENGINE_ERROR_UNIMPLEMENTED();

				renderTargetIndex += 1;

				continue;
			}

			const Attachment& colorAttachment = renderPass->getAttachment(colorAttachmentReference.index);
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[colorAttachmentReference.index]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
				presentRenderTargetIndex = colorAttachmentReference.index;

				if (colorAttachment.loadOperation != LoadOperation::CLEAR && image->resourceState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
					D3D12_RESOURCE_BARRIER resourceBarrier{};
					resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					resourceBarrier.Transition.pResource = image->resource;
					resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					resourceBarrier.Transition.StateBefore = image->resourceState;
					resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					image->resourceState = resourceBarrier.Transition.StateAfter;

					resourceBarriers.emplace_back(resourceBarrier);
				}
			}

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && colorAttachment.loadOperation != LoadOperation::CLEAR) {
				D3D12_RESOURCE_BARRIER resourceBarrier{};
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource;
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				resourceBarrier.Transition.StateBefore = image->resourceState;
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				image->resourceState = resourceBarrier.Transition.StateAfter;

				resourceBarriers.emplace_back(resourceBarrier);
			}

			renderTargetViewHandles[colorAttachmentIndex] = imageView->descriptorHandle;
			colorAttachmentIndex += 1;
			renderTargetIndex += 1;
		}

		if (resourceBarriers.size() > 0) {
			commandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.size()), &resourceBarriers[0]);
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilViewHandlePointer = nullptr;

		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle{};

		if (subpass->hasDepthStencilAttachmentReference()) {
			const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[depthStencilAttachmentReference.index]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && renderPass->getAttachment(depthStencilAttachmentReference.index).loadOperation != LoadOperation::CLEAR) {
				D3D12_RESOURCE_BARRIER resourceBarrier{};
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource;
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				resourceBarrier.Transition.StateBefore = image->resourceState;
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;

				image->resourceState = resourceBarrier.Transition.StateAfter;

				commandList->ResourceBarrier(1, &resourceBarrier);
			}

			depthStencilViewHandle = imageView->descriptorHandle;
			depthStencilViewHandlePointer = &depthStencilViewHandle;
		}

		commandList->OMSetRenderTargets(renderTargetCount, &renderTargetViewHandles[0], false, depthStencilViewHandlePointer);
	}

	ref<Trident::CommandEncoder> DX12::createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor) {
		// TODO
		BREAKPOINT();

		return createRef<DX12::CommandEncoder>(device, commandPool, descriptor);
	}

	void DX12::destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder) {
		const DX12::CommandEncoder* _commandEncoder = static_cast<const DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->commandList->Release();
	}
}