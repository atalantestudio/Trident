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

			void bindComputePipeline(const ref<ProjectA::ComputePipeline>& computePipeline) final override;

			void bindRenderPipeline(const ref<ProjectA::RenderPipeline>& renderPipeline) final override;

			void bindDescriptorSets(const ref<ProjectA::PipelineLayout>& pipelineLayout, const sequence<ref<ProjectA::DescriptorSet>>& descriptorSets) final override;

			void bindVertexBuffer(uint32 bindingIndex, const ref<ProjectA::Buffer>& vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(const ref<ProjectA::Buffer>& sourceBuffer, const ref<ProjectA::Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) final override;

			void copyBufferToImage(const ref<ProjectA::Buffer>& sourceBuffer, const ref<ProjectA::Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void copyImageToBuffer(const ref<ProjectA::Image>& sourceImage, const ref<ProjectA::Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void dispatchIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset) final override;

			void nextSubpass() final override;

			void setImageMemoryBarrier(const ref<ProjectA::Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) final override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) final override;

			void drawIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) final override;

			void drawIndexedIndirect(const ref<ProjectA::Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) final override;

			void endRenderPass() final override;

			ref<ProjectA::CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(const ref<ProjectA::Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

		private:
			// ComPtr<ID3D12CommandList> commandList;
	};
}