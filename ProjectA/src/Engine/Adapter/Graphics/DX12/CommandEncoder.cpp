// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceStates.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndirectCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	DX12::CommandEncoder::CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool) :
		device(device),
		commandAllocator(commandPool.commandAllocator)
	{
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
		const ref<DX12::Swapchain> swapchain = std::static_pointer_cast<DX12::Swapchain>(Renderer::getSwapchain());

		// TODO: Set correct resource barriers.
		D3D12_RESOURCE_BARRIER resourceBarrier{};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = swapchain->getImage(Renderer::getImageIndex())->resource.Get();
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		commandList->ResourceBarrier(1, &resourceBarrier);

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewDescriptorHandle = swapchain->renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		renderTargetViewDescriptorHandle.ptr += Renderer::getImageIndex() * swapchain->renderTargetViewDescriptorSize;

		commandList->OMSetRenderTargets(1, &renderTargetViewDescriptorHandle, false, nullptr);

		ADAPTER_ASSERT(descriptor.clearValues.count() == 1, "Too many clear values.");

		D3D12_RECT rectangle{};
		rectangle.left = 0;
		rectangle.top = 0;
		rectangle.right = swapchain->getExtent().width;
		rectangle.bottom = swapchain->getExtent().height;

		commandList->ClearRenderTargetView(renderTargetViewDescriptorHandle, descriptor.clearValues[0].color, 1, &rectangle);

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindComputePipeline(ref<ProjectA::ComputePipeline> computePipeline) {
		const ref<DX12::ComputePipeline> pipeline = std::static_pointer_cast<DX12::ComputePipeline>(computePipeline);

		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetComputeRootSignature(pipeline->pipelineLayout->rootSignature.Get());

		computePipelineBound = true;
		commandCount += 1;
	}

	void DX12::CommandEncoder::bindRenderPipeline(ref<ProjectA::RenderPipeline> renderPipeline) {
		const ref<DX12::RenderPipeline> pipeline = std::static_pointer_cast<DX12::RenderPipeline>(renderPipeline);

		commandList->SetPipelineState(pipeline->pipelineState.Get());
		commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature.Get());
		commandList->IASetPrimitiveTopology(pipeline->primitiveTopology);

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindDescriptorSets(ref<ProjectA::PipelineLayout> pipelineLayout, const sequence<ref<ProjectA::DescriptorSet>>& descriptorSets) {
		// TODO: Replace with sequence<T>.
		std::vector<ID3D12DescriptorHeap*> descriptorHeaps;

		UINT descriptorSetIndex = 0;

		while (descriptorSetIndex < descriptorSets.count()) {
			const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[descriptorSetIndex]);

			if (descriptorSet->cbvSrvUavViewDescriptorHeap) {
				descriptorHeaps.emplace_back(descriptorSet->cbvSrvUavViewDescriptorHeap.Get());
			}

			if (descriptorSet->samplerDescriptorHeap) {
				descriptorHeaps.emplace_back(descriptorSet->samplerDescriptorHeap.Get());
			}

			descriptorSetIndex += 1;
		}

		commandList->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()), &descriptorHeaps[0]);

		descriptorSetIndex = 0;

		UINT rootParameterIndex = 0;

		if (computePipelineBound) {
			while (descriptorSetIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[descriptorSetIndex]);

				if (descriptorSet->cbvSrvUavViewDescriptorHeap) {
					commandList->SetComputeRootDescriptorTable(rootParameterIndex, descriptorSet->cbvSrvUavViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

					rootParameterIndex += 1;
				}

				if (descriptorSet->samplerDescriptorHeap) {
					commandList->SetComputeRootDescriptorTable(rootParameterIndex, descriptorSet->samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

					rootParameterIndex += 1;
				}

				descriptorSetIndex += 1;
			}
		}
		else {
			while (descriptorSetIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[descriptorSetIndex]);

				if (descriptorSet->cbvSrvUavViewDescriptorHeap) {
					commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorSet->cbvSrvUavViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

					rootParameterIndex += 1;
				}

				if (descriptorSet->samplerDescriptorHeap) {
					commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorSet->samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

					rootParameterIndex += 1;
				}

				descriptorSetIndex += 1;
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<ProjectA::Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		ADAPTER_ASSERT(offset < vertexBuffer->size, "The vertex buffer offset is overflowing.");

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(vertexBuffer)->resource->GetGPUVirtualAddress() + offset;
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer->size - offset);
		vertexBufferView.StrideInBytes = stride;

		commandList->IASetVertexBuffers(bindingIndex, 1, &vertexBufferView);

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToBuffer(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		const ref<DX12::Buffer> srcBuffer = std::static_pointer_cast<DX12::Buffer>(sourceBuffer);
		const ref<DX12::Buffer> dstBuffer = std::static_pointer_cast<DX12::Buffer>(destinationBuffer);

		for (const BufferCopyRegion& copyRegion : copyRegions) {
			commandList->CopyBufferRegion(dstBuffer->resource.Get(), copyRegion.destinationOffset, srcBuffer->resource.Get(), copyRegion.sourceOffset, copyRegion.size);
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyBufferToImage(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
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

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			bufferCopyLocation.PlacedFootprint.Footprint.Width = dstImage->getExtent().width;
			bufferCopyLocation.PlacedFootprint.Footprint.Height = dstImage->getExtent().height;
			bufferCopyLocation.PlacedFootprint.Footprint.Depth = dstImage->getExtent().depth;
			bufferCopyLocation.PlacedFootprint.Footprint.RowPitch = bufferCopyLocation.PlacedFootprint.Footprint.Width * channelCount;

			UINT layerIndex = copyRegion.imageLayerOffset;

			while (layerIndex < copyRegion.imageLayerCount) {
				bufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset + dstImage->getExtent().width * dstImage->getExtent().height * dstImage->getExtent().depth * channelCount * layerIndex;

				textureCopyLocation.SubresourceIndex = layerIndex;

				commandList->CopyTextureRegion(&textureCopyLocation, copyRegion.imageOffsetX, copyRegion.imageOffsetY, copyRegion.imageOffsetZ, &bufferCopyLocation, nullptr);

				layerIndex += 1;
			}
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyImageToBuffer(ref<ProjectA::Image> sourceImage, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		commandList->Dispatch(groupCountX, groupCountY, groupCountZ);

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatchIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor = {};
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
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::setImageMemoryBarrier(ref<ProjectA::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		if (image->getLayout() == newLayout) {
			ADAPTER_WARNING("Ignored transition resource barrier to the same layout.");

			return;
		}

		if (newLayout == ImageLayout::TRANSFER_DST_OPTIMAL) {
			// Transitions to D3D12_RESOURCE_STATE_COPY_DEST are handled implicitly.
			return;
		}

		if (newLayout == ImageLayout::SHADER_READ_ONLY_OPTIMAL && computePipelineBound) {
			// This transition can't be execute by the compute queue.
			return;
		}

		D3D12_RESOURCE_BARRIER resourceBarrier{};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = std::static_pointer_cast<DX12::Image>(image)->resource.Get();
		resourceBarrier.Transition.Subresource = 0;
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

	void DX12::CommandEncoder::drawIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor = {};
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

	void DX12::CommandEncoder::drawIndexedIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		const ref<DX12::Buffer> indirectBuffer = std::static_pointer_cast<DX12::Buffer>(buffer);

		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor = {};
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
		const ref<DX12::Swapchain> swapchain = std::static_pointer_cast<DX12::Swapchain>(Renderer::getSwapchain());

		// TODO: Set correct resource barriers.
		D3D12_RESOURCE_BARRIER resourceBarrier{};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = swapchain->getImage(Renderer::getImageIndex())->resource.Get();
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		commandList->ResourceBarrier(1, &resourceBarrier);

		commandCount += 1;
	}

	ref<ProjectA::CommandBuffer> DX12::CommandEncoder::end() const {
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
	}

	void DX12::CommandEncoder::bindIndexBuffer(ref<ProjectA::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		ADAPTER_ASSERT(offset < indexBuffer->size, "The index buffer offset is overflowing.");

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = std::static_pointer_cast<DX12::Buffer>(indexBuffer)->resource->GetGPUVirtualAddress() + offset;
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer->size - offset);
		indexBufferView.Format = getIndexBufferFormat(indexTypeSize);

		commandList->IASetIndexBuffer(&indexBufferView);

		commandCount += 1;
	}
}