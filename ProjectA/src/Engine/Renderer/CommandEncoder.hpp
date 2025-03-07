#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/IndexType.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;
		ref<Swapchain> swapchain;
		vec4<float32> clearColor;
		uint32 nextImageIndex;
	};

	class CommandEncoder {
		public:
			virtual ~CommandEncoder() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) = 0;

			virtual void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) = 0;

			virtual void bindDescriptorSets(const uint32 descriptorSetIndex) const = 0;

			virtual void bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) = 0;

			virtual void bindVertexBuffer(const uint32 bindingIndex, const ref<Buffer>& vertexBuffer, const uint32 stride) = 0;

			virtual void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) = 0;

			virtual void setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) = 0;

			virtual void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) = 0;

			virtual void drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) = 0;

			virtual void endRenderPass() const = 0;

			virtual ref<CommandBuffer> end() const = 0;

			virtual void reset() = 0;
	};
}