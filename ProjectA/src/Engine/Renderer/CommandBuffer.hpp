#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;
		ref<Swapchain> swapchain;
		vec4<float32> clearColor;
		float32 depth;
		uint32 stencil;
		uint32 nextImageIndex;
	};

	class CommandBuffer {
		public:
			virtual ~CommandBuffer() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) const = 0;

			virtual void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) const = 0;

			virtual void bindVertexBuffer(const ref<Buffer>& vertexBuffer) const = 0;

			virtual void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) const = 0;

			virtual void setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) const = 0;

			virtual void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) const = 0;

			virtual void endRenderPass() const = 0;

			virtual void end() const = 0;

			virtual void reset() const = 0;
	};
}