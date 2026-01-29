// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceStates.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndirectCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	DX12::CommandEncoder::CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool) :
		device(device),
		commandAllocator(commandPool.commandAllocator)
	{
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(descriptor.usage != CommandBufferUsage::SIMULTANEOUS_USE, "SIMULTANEOUS_USE is not supported on command lists.");

		HRESULT result;

		result = device.device->CreateCommandList(0, commandPool.type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command list.");

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

		sequence<D3D12_RESOURCE_BARRIER> resourceBarriers(renderPass->getAttachmentCount());

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[attachmentIndex]);

			D3D12_RESOURCE_BARRIER& resourceBarrier = resourceBarriers[attachmentIndex];
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = imageView->image->resource.Get();
			resourceBarrier.Transition.Subresource = imageView->getLayerOffset();
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;

			if (isDepthFormat(imageView->image->getFormat())) {
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			}
			else {
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			}

			attachmentIndex += 1;
		}

		commandList->ResourceBarrier(static_cast<UINT>(resourceBarriers.count()), &resourceBarriers[0]);

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
				commandList->ClearDepthStencilView(imageView->cpuDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearValue.depth, static_cast<UINT8>(clearValue.stencil), 1, &rectangle);
			}
			else {
				commandList->ClearRenderTargetView(imageView->cpuDescriptorHandle, clearValue.color, 1, &rectangle);
			}

			attachmentIndex += 1;
		}

		setRenderTargets();

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindComputePipeline(ref<Trident::ComputePipeline> computePipeline) {
		const ref<DX12::ComputePipeline> pipeline = std::static_pointer_cast<DX12::ComputePipeline>(computePipeline);

		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetComputeRootSignature(pipeline->pipelineLayout->rootSignature.Get());

		computePipelineBound = true;
		commandCount += 1;
	}

	void DX12::CommandEncoder::bindRenderPipeline(ref<Trident::RenderPipeline> renderPipeline) {
		const ref<DX12::RenderPipeline> pipeline = std::static_pointer_cast<DX12::RenderPipeline>(renderPipeline);

		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature.Get());
		commandList->IASetPrimitiveTopology(pipeline->primitiveTopology);

		samplerDescriptorTable = pipeline->pipelineLayout->samplerDescriptorTable;

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindDescriptorSets(ref<Trident::PipelineLayout> pipelineLayout, const sequence<ref<Trident::DescriptorSet>>& descriptorSets) {
		ID3D12DescriptorHeap* viewDescriptorHeap = device.memoryAllocator->getCurrentViewDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* samplerDescriptorHeap = device.memoryAllocator->getCurrentSamplerDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* descriptorHeaps[2] = {viewDescriptorHeap, samplerDescriptorHeap};

		commandList->SetDescriptorHeaps(2, &descriptorHeaps[0]);

		UINT rootParameterIndex = 0;

		if (computePipelineBound) {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * device.memoryAllocator->viewDescriptorHandleIncrementSize;

				commandList->SetComputeRootDescriptorTable(rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (samplerDescriptorTable) {
				// Bind sampler descriptor table to the last root parameter.
				commandList->SetComputeRootDescriptorTable(rootParameterIndex, samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			}
		}
		else {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * device.memoryAllocator->viewDescriptorHandleIncrementSize;

				commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (samplerDescriptorTable) {
				// Bind sampler descriptor table to the last root parameter.
				commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		ADAPTER_ASSERT(offset < vertexBuffer->size, "The vertex buffer offset is overflowing.");

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(vertexBuffer)->resource->GetGPUVirtualAddress() + offset;
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer->size - offset);
		vertexBufferView.StrideInBytes = stride;

		commandList->IASetVertexBuffers(bindingIndex, 1, &vertexBufferView);

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToBuffer(ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		const ref<DX12::Buffer> srcBuffer = std::static_pointer_cast<DX12::Buffer>(sourceBuffer);
		const ref<DX12::Buffer> dstBuffer = std::static_pointer_cast<DX12::Buffer>(destinationBuffer);

		// NOTE
		// D3D12_HEAP_TYPE_UPLOAD heaps only allow CPU writes.
		// The source and destination buffers are mapped to emulate a GPU copy command.
		if (dstBuffer->heapType == D3D12_HEAP_TYPE_UPLOAD) {
			uint8* sourceMappedRange = srcBuffer->map<uint8>(0, srcBuffer->size);
			uint8* destinationMappedRange = dstBuffer->map<uint8>(0, dstBuffer->size);

			for (const BufferCopyRegion& copyRegion : copyRegions) {
				std::copy_n(sourceMappedRange + copyRegion.sourceOffset, copyRegion.size, destinationMappedRange + copyRegion.destinationOffset);
			}

			srcBuffer->unmap();
			dstBuffer->unmap();
		}
		else {
			for (const BufferCopyRegion& copyRegion : copyRegions) {
				commandList->CopyBufferRegion(dstBuffer->resource.Get(), copyRegion.destinationOffset, srcBuffer->resource.Get(), copyRegion.sourceOffset, copyRegion.size);
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToImage(ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
		const ref<DX12::Buffer> srcBuffer = std::static_pointer_cast<DX12::Buffer>(sourceBuffer);
		const ref<DX12::Image> dstImage = std::static_pointer_cast<DX12::Image>(destinationImage);
		const D3D12_RESOURCE_DESC srcBufferResourceDescriptor = srcBuffer->resource->GetDesc();
		const D3D12_RESOURCE_DESC dstImageResourceDescriptor = dstImage->resource->GetDesc();
		const UINT channelCount = getChannelCount(dstImage->getFormat());

		D3D12_TEXTURE_COPY_LOCATION bufferCopyLocation{};
		bufferCopyLocation.pResource = srcBuffer->resource.Get();
		bufferCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		bufferCopyLocation.PlacedFootprint.Footprint.Format = dstImageResourceDescriptor.Format;

		D3D12_TEXTURE_COPY_LOCATION textureCopyLocation{};
		textureCopyLocation.pResource = dstImage->resource.Get();
		textureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		UINT layerIndex;

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			const UINT64 layerSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * channelCount;

			bufferCopyLocation.PlacedFootprint.Footprint.Width = copyRegion.imageExtent.width;
			bufferCopyLocation.PlacedFootprint.Footprint.Height = copyRegion.imageExtent.height;
			bufferCopyLocation.PlacedFootprint.Footprint.Depth = copyRegion.imageExtent.depth;
			bufferCopyLocation.PlacedFootprint.Footprint.RowPitch = copyRegion.imageExtent.width * channelCount;

			layerIndex = 0;

			while (layerIndex < copyRegion.imageLayerCount) {
				bufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset + layerSize * layerIndex;

				textureCopyLocation.SubresourceIndex = copyRegion.imageLayerOffset + layerIndex;

				commandList->CopyTextureRegion(&textureCopyLocation, copyRegion.imageOffsetX, copyRegion.imageOffsetY, copyRegion.imageOffsetZ, &bufferCopyLocation, nullptr);

				layerIndex += 1;
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyImageToBuffer(ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		commandList->Dispatch(groupCountX, groupCountY, groupCountZ);

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatchIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NumArgumentDescs = 1;
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndirectCommand);

			const HRESULT result = device.device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature.Get(),
			1,
			std::static_pointer_cast<DX12::Buffer>(buffer)->resource.Get(),
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

	void DX12::CommandEncoder::setImageMemoryBarrier(ref<Trident::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		if (image->getLayout() == newLayout) {
			ADAPTER_WARNING("Ignored transition resource barrier to the same layout.");

			return;
		}

		if (computePipelineBound && newLayout == ImageLayout::SHADER_READ_ONLY_OPTIMAL) {
			// This transition can't be executed by the compute queue.
			return;
		}

		D3D12_RESOURCE_BARRIER resourceBarrier{};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = std::static_pointer_cast<DX12::Image>(image)->resource.Get();
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		resourceBarrier.Transition.StateBefore = getImageTransitionResourceStates(image->getLayout());
		resourceBarrier.Transition.StateAfter = getImageTransitionResourceStates(newLayout);

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
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NumArgumentDescs = 1;
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndirectCommand);

			const HRESULT result = device.device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature.Get(),
			drawCount,
			std::static_pointer_cast<DX12::Buffer>(buffer)->resource.Get(),
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
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NumArgumentDescs = 1;
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndexedIndirectCommand);

			const HRESULT result = device.device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		commandList->ExecuteIndirect(
			indirectBuffer->commandSignature.Get(),
			drawCount,
			std::static_pointer_cast<DX12::Buffer>(buffer)->resource.Get(),
			bufferOffset,
			nullptr,
			0
		);

		commandCount += 1;
	}

	void DX12::CommandEncoder::endRenderPass() {
		if (presentRenderTargetIndex < framebuffer->imageViews.count()) {
			ref<DX12::ImageView> presentImageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[presentRenderTargetIndex]);
			D3D12_RESOURCE_STATES presentRenderTargetResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

			if (presentImageView->image->getSampleCount() > 1) {
				const ref<Subpass> subpass = framebuffer->getRenderPass()->getSubpass(subpassIndex);
				const ref<DX12::ImageView> resolveImageView = presentImageView;

				presentImageView = std::static_pointer_cast<DX12::ImageView>(framebuffer->imageViews[subpass->getResolveAttachmentReferenceCount() + presentRenderTargetIndex]);

				D3D12_RESOURCE_BARRIER resourceBarriers[2];

				D3D12_RESOURCE_BARRIER& resolveResourceBarrier = resourceBarriers[0];
				resolveResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resolveResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resolveResourceBarrier.Transition.pResource = resolveImageView->image->resource.Get();
				resolveResourceBarrier.Transition.Subresource = resolveImageView->getLayerOffset();
				resolveResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				resolveResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

				D3D12_RESOURCE_BARRIER& presentResourceBarrier = resourceBarriers[1];
				presentResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				presentResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				presentResourceBarrier.Transition.pResource = presentImageView->image->resource.Get();
				presentResourceBarrier.Transition.Subresource = presentImageView->getLayerOffset();
				presentResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				presentResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

				commandList->ResourceBarrier(2, &resourceBarriers[0]);

				presentRenderTargetResourceState = D3D12_RESOURCE_STATE_RESOLVE_DEST;

				commandList->ResolveSubresource(
					presentImageView->image->resource.Get(),
					presentImageView->getLayerOffset(),
					resolveImageView->image->resource.Get(),
					resolveImageView->getLayerOffset(),
					getImageFormat(presentImageView->image->getFormat())
				);
			}

			D3D12_RESOURCE_BARRIER resourceBarrier{};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = presentImageView->image->resource.Get();
			resourceBarrier.Transition.Subresource = presentImageView->getLayerOffset();
			resourceBarrier.Transition.StateBefore = presentRenderTargetResourceState;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

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
		HRESULT result;

		result = commandAllocator->Reset();

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command allocator.");

		result = commandList->Reset(commandAllocator.Get(), nullptr);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command list.");

		commandCount = 0;
		computePipelineBound = false;
		framebuffer = nullptr;
		subpassIndex = 0;
		presentRenderTargetIndex = max<UINT>();
		samplerDescriptorTable = false;
	}

	void DX12::CommandEncoder::bindIndexBuffer(ref<Trident::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		ADAPTER_ASSERT(offset < indexBuffer->size, "The index buffer offset is overflowing.");

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(indexBuffer)->resource->GetGPUVirtualAddress() + offset;
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer->size - offset);
		indexBufferView.Format = getIndexBufferFormat(indexTypeSize);

		commandList->IASetIndexBuffer(&indexBufferView);

		commandCount += 1;
	}

	void DX12::CommandEncoder::setRenderTargets() {
		// NOTE
		// On the first subpass, all attachments that need to be cleared (even those not used by this subpass)
		// have been transitioned into:
		// - RENDER_TARGET for color attachments
		// - DEPTH_WRITE for depth/stencil attachments.

		presentRenderTargetIndex = max<UINT>();

		const ref<RenderPass> renderPass = framebuffer->getRenderPass();
		const ref<Subpass> subpass = renderPass->getSubpass(subpassIndex);
		const sequence<AttachmentReference>& colorAttachmentReferences = subpass->getColorAttachmentReferences();

		if (subpass->getInputAttachmentReferenceCount() > 0) {
			const sequence<AttachmentReference>& inputAttachmentReferences = subpass->getInputAttachmentReferences();

			sequence<D3D12_RESOURCE_BARRIER> resourceBarriers(subpass->getInputAttachmentReferenceCount());
			UINT inputAttachmentReferenceIndex = 0;

			while (inputAttachmentReferenceIndex < inputAttachmentReferences.count()) {
				const AttachmentReference& inputAttachmentReference = inputAttachmentReferences[inputAttachmentReferenceIndex];

				if (inputAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					ENGINE_ERROR_UNIMPLEMENTED();

					inputAttachmentReferenceIndex += 1;

					continue;
				}

				const DX12::Image* image = static_cast<const DX12::Image*>(framebuffer->imageViews[inputAttachmentReference.index]->getImage());

				D3D12_RESOURCE_BARRIER& resourceBarrier = resourceBarriers[inputAttachmentReferenceIndex];
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource.Get();
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

				if (isDepthFormat(image->getFormat())) {
					resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
				}
				else {
					resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				}

				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

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
			const DX12::Image* image = static_cast<const DX12::Image*>(imageView->getImage());

			if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
				presentRenderTargetIndex = colorAttachmentReference.index;

				if (colorAttachment.loadOperation != LoadOperation::CLEAR) {
					D3D12_RESOURCE_BARRIER resourceBarrier{};
					resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					resourceBarrier.Transition.pResource = image->resource.Get();
					resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

					if (image->getSampleCount() > 1) {
						resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
					}
					else {
						resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
					}

					resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					resourceBarriers.emplace_back(resourceBarrier);
				}
			}

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && colorAttachment.loadOperation != LoadOperation::CLEAR) {
				D3D12_RESOURCE_BARRIER resourceBarrier{};
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource.Get();
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				resourceBarriers.emplace_back(resourceBarrier);
			}

			renderTargetViewHandles[colorAttachmentIndex] = imageView->cpuDescriptorHandle;
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
			const DX12::Image* image = static_cast<const DX12::Image*>(imageView->getImage());

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && renderPass->getAttachment(depthStencilAttachmentReference.index).loadOperation != LoadOperation::CLEAR) {
				D3D12_RESOURCE_BARRIER resourceBarrier{};
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = image->resource.Get();
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;

				commandList->ResourceBarrier(1, &resourceBarrier);
			}

			depthStencilViewHandle = imageView->cpuDescriptorHandle;
			depthStencilViewHandlePointer = &depthStencilViewHandle;
		}

		commandList->OMSetRenderTargets(renderTargetCount, &renderTargetViewHandles[0], false, depthStencilViewHandlePointer);
	}
}