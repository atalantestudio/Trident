#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/CommandBufferUsage.hpp"
#include "Engine/Renderer/ComputePipeline.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Engine/Renderer/ImageLayout.hpp"
#include "Engine/Renderer/Pipeline.hpp"
#include "Engine/Renderer/PipelineStage.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	struct CommandBufferDescriptor {
		CommandBufferUsage usage;
	};

	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;
		vec4<float32> clearColor;
		uint32 nextImageIndex = 0;
	};

	class CommandEncoder {
		public:
			virtual ~CommandEncoder() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) = 0;

			// TODO: Merge bindComputePipeline and bindRenderPipeline?
			virtual void bindComputePipeline(const ref<ComputePipeline>& computePipeline) = 0;

			virtual void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) = 0;

			// Binds all descriptor sets linked to the current pipeline.
			// TODO: Change the API to take in argument arbitrary descriptor sets .
			virtual void bindAllDescriptorSets() = 0;

			template<typename IndexType>
			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset);

			// `stride` is only used by OpenGL when calling glBindVertexBuffer.
			virtual void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) = 0;

			virtual void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) = 0;

			virtual void copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, uint32 width, uint32 height, uint32 depth) = 0;

			virtual void dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;

			virtual void setImageMemoryBarrier(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) = 0;

			virtual void setLineWidth(float32 lineWidth) = 0;

			// Updates the scissor area.
			// Should only be used if the render pipeline's dynamic state contains DynamicState::SCISSOR.
			virtual void setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) = 0;

			// Updates the viewport area.
			// Should only be used if the render pipeline's dynamic state contains DynamicState::VIEWPORT.
			virtual void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) = 0;

			virtual void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) = 0;

			virtual void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) = 0;

			virtual void endRenderPass() const = 0;

			virtual ref<CommandBuffer> end() const = 0;

			virtual void reset() = 0;

		protected:
			virtual void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) = 0;
	};
}