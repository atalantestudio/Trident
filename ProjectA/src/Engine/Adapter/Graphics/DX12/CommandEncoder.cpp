// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceStates.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	DX12::CommandEncoder::CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool) :
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
		ENGINE_ERROR_UNIMPLEMENTED();

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
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<ProjectA::Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		const ComPtr<ID3D12Resource>& resource = std::static_pointer_cast<DX12::Buffer>(vertexBuffer)->resource;

		ADAPTER_ASSERT(offset < resource->GetDesc().Width, "The vertex buffer bind offset is overflowing.");

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = resource->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(resource->GetDesc().Width - offset);
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

		D3D12_TEXTURE_COPY_LOCATION bufferCopyLocation{};
		bufferCopyLocation.pResource = srcBuffer->resource.Get();
		bufferCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		bufferCopyLocation.PlacedFootprint.Footprint.Format = dstImageResourceDescriptor.Format;
		bufferCopyLocation.PlacedFootprint.Footprint.Width = static_cast<UINT>(dstImageResourceDescriptor.Width);
		bufferCopyLocation.PlacedFootprint.Footprint.Height = srcBufferResourceDescriptor.Height;
		bufferCopyLocation.PlacedFootprint.Footprint.Depth = srcBufferResourceDescriptor.DepthOrArraySize;
		bufferCopyLocation.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(srcBufferResourceDescriptor.Width);

		D3D12_TEXTURE_COPY_LOCATION textureCopyLocation{};
		textureCopyLocation.pResource = dstImage->resource.Get();
		textureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			bufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset;

			textureCopyLocation.SubresourceIndex = copyRegion.imageMipLevel + copyRegion.imageLayerOffset;

			commandList->CopyTextureRegion(&textureCopyLocation, copyRegion.imageOffsetX, copyRegion.imageOffsetY, copyRegion.imageOffsetZ, &bufferCopyLocation, nullptr);
		}

		commandCount += 1;
	}

	void DX12::CommandEncoder::copyImageToBuffer(ref<ProjectA::Image> sourceImage, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::dispatchIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset) {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::nextSubpass() {
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::setImageMemoryBarrier(ref<ProjectA::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		if (newLayout == ImageLayout::TRANSFER_DST_OPTIMAL) {
			// Transitions to D3D12_RESOURCE_STATE_COPY_DEST are handled implicitly.
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
		ENGINE_ERROR_UNIMPLEMENTED();

		commandCount += 1;
	}

	void DX12::CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		commandList->DrawIndexedInstanced(indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		commandCount += 1;
	}

	void DX12::CommandEncoder::drawIndexedIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ENGINE_ERROR_UNIMPLEMENTED();

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
	}

	void DX12::CommandEncoder::bindIndexBuffer(ref<ProjectA::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(indexBuffer);

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = buffer->resource->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(offset - buffer->size);
		indexBufferView.Format = getIndexBufferFormat(indexTypeSize);

		commandList->IASetIndexBuffer(&indexBufferView);

		commandCount += 1;
	}
}