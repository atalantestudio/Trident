// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPipeline.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace Trident {
	class GL::CommandEncoder final : public Trident::CommandEncoder {
		public:
			explicit CommandEncoder(const CommandBufferDescriptor& descriptor, const GL::Device& device);

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(ref<Trident::ComputePipeline> computePipeline) final override;

			void bindRenderPipeline(ref<Trident::RenderPipeline> renderPipeline) final override;

			void bindDescriptorSets(ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) final override;

			void bindVertexBuffer(uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) final override;

			void copyBufferToImage(ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) final override;

			void copyImageToBuffer(ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) final override;

			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void dispatchIndirect(ref<Trident::Buffer> buffer, uint64 bufferOffset) final override;

			void nextSubpass() final override;

			void insertBufferMemoryBarriers(view<BufferMemoryBarrier> barriers) final override {}

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

		private:
			ref<GL::Framebuffer> framebuffer = nullptr;
			ref<GL::RenderPass> renderPass = nullptr;
			uint32 subpassIndex = 0;
			std::vector<ref<GL::Command>> commands;
			ref<GL::ComputePipeline> computePipeline = nullptr;
			ref<GL::RenderPipeline> renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;
			PhysicalDeviceLimits limits;
	};
}