// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/CommandEncoder.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::CommandEncoder::CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;
		ComPtr<ID3D12CommandList> commandList;

		result = device.device->CreateCommandList(0, commandPool.type, commandPool.commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command list.");

		// TODO: Store returned command list.
	}

	void DX12::CommandEncoder::begin() const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::bindComputePipeline(const ref<ProjectA::ComputePipeline>& computePipeline) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::bindRenderPipeline(const ref<ProjectA::RenderPipeline>& renderPipeline) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::bindDescriptorSets(const ref<ProjectA::PipelineLayout>& pipelineLayout, const sequence<ref<ProjectA::DescriptorSet>>& descriptorSets) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<ProjectA::Buffer>& vertexBuffer, uint64 offset, uint32 stride) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::copyBufferToBuffer(const ref<ProjectA::Buffer>& sourceBuffer, const ref<ProjectA::Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::copyBufferToImage(const ref<ProjectA::Buffer>& sourceBuffer, const ref<ProjectA::Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::copyImageToBuffer(const ref<ProjectA::Image>& sourceImage, const ref<ProjectA::Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::dispatchIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::nextSubpass() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::setImageMemoryBarrier(const ref<ProjectA::Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::setLineWidth(float32 lineWidth) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::drawIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::drawIndexedIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::endRenderPass() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	ref<ProjectA::CommandBuffer> DX12::CommandEncoder::end() const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	void DX12::CommandEncoder::reset() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::CommandEncoder::bindIndexBuffer(const ref<ProjectA::Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}
}