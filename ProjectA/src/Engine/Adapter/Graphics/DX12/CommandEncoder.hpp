// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class DX12::CommandEncoder final : public ProjectA::CommandEncoder {
		public:
			explicit CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool);

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(ref<ProjectA::ComputePipeline> computePipeline) final override;

			void bindRenderPipeline(ref<ProjectA::RenderPipeline> renderPipeline) final override;

			void bindDescriptorSets(ref<ProjectA::PipelineLayout> pipelineLayout, const sequence<ref<ProjectA::DescriptorSet>>& descriptorSets) final override;

			void bindVertexBuffer(uint32 bindingIndex, ref<ProjectA::Buffer> vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) final override;

			void copyBufferToImage(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void copyImageToBuffer(ref<ProjectA::Image> sourceImage, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void dispatchIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset) final override;

			void nextSubpass() final override;

			void setImageMemoryBarrier(ref<ProjectA::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) final override;

			void setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) final override;

			void drawIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) final override;

			void drawIndexedIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void endRenderPass() final override;

			ref<ProjectA::CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(ref<ProjectA::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

		private:
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			ComPtr<ID3D12GraphicsCommandList> commandList;
			uint32 commandCount = 0;
	};
}