// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace Trident {
	class DX12::CommandEncoder final : public Trident::CommandEncoder {
		public:
			explicit CommandEncoder(const CommandBufferDescriptor& descriptor, const DX12::Device& device, const DX12::CommandPool& commandPool);

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(ref<Trident::ComputePipeline> computePipeline) final override;

			void bindRenderPipeline(ref<Trident::RenderPipeline> renderPipeline) final override;

			void bindDescriptorSets(ref<Trident::PipelineLayout> pipelineLayout, const sequence<ref<Trident::DescriptorSet>>& descriptorSets) final override;

			void bindVertexBuffer(uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) final override;

			void copyBufferToImage(ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void copyImageToBuffer(ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void dispatchIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset) final override;

			void nextSubpass() final override;

			void setImageMemoryBarrier(ref<Trident::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) final override;

			void setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) final override;

			void drawIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) final override;

			void drawIndexedIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void endRenderPass() final override;

			ref<Trident::CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(ref<Trident::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

			void setRenderTargets();

		private:
			const DX12::Device& device;
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			ComPtr<ID3D12GraphicsCommandList> commandList;
			uint32 commandCount = 0;
			bool computePipelineBound = false;
			ref<DX12::Framebuffer> framebuffer = nullptr;
			UINT subpassIndex = 0;
			UINT presentRenderTargetIndex = max<UINT>();
	};
}