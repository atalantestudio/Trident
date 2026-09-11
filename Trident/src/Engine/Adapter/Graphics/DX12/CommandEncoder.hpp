// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Framebuffer.hpp"

namespace Trident {
	class DX12::CommandEncoder final : public Trident::CommandEncoder {
		public:
			explicit CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool);

		public:
			ID3D12GraphicsCommandList7* commandList;
			ID3D12CommandAllocator* commandAllocator;
			CommandBufferUsage usage;
			UINT commandCount = 0;
			const DX12::GraphicsPipeline* graphicsPipeline = nullptr;
			UINT samplerDescriptorTableRootParameterIndex = max<UINT>();
			bool computeCommandList;
			bool computePipelineBound = false;
			ref<Framebuffer> framebuffer = nullptr;
			UINT subpassIndex = 0;
			UINT presentRenderTargetIndex = max<UINT>();
	};

	namespace DX12 {
		void begin(const Trident::CommandEncoder* commandEncoder);

		void beginRenderPass(Trident::CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor);

		void bindComputePipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::ComputePipeline> computePipeline);

		void bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets);

		void bindGraphicsPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::GraphicsPipeline> graphicsPipeline);

		void bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> indexBuffer, uint64 offset, IndexFormat format);

		void bindVertexBuffer(Trident::CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset);

		void copyBufferToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions);

		void copyBufferToImage(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions);

		void copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions);

		ref<Trident::CommandEncoder> createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor);

		void destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder);

		void dispatch(Trident::CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

		void dispatchIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset);

		void draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset);

		void drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset);

		void drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

		void drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

		ref<Trident::CommandBuffer> end(const Trident::CommandEncoder* commandEncoder);

		void endRenderPass(Trident::CommandEncoder* commandEncoder);

		void insertMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers);

		void nextSubpass(Trident::CommandEncoder* commandEncoder);

		void reset(Trident::CommandEncoder* commandEncoder);

		void setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth);

		void setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent);

		void setViewport(Trident::CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth);
	}
}