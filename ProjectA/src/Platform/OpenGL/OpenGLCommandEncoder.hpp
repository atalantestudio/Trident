#pragma once

#include "Engine/Renderer/CommandEncoder.hpp"

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"
#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLCommandEncoder : public CommandEncoder {
		public:
			OpenGLCommandEncoder();

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) override;

			void bindVertexBuffer(const ref<Buffer>& vertexBuffer, const uint32 stride) override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) override;

			void setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) override;

			void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		private:
			const OpenGLRenderPipeline* renderPipeline = nullptr;
			std::vector<ref<OpenGLCommand>> commandBuffer;
	};
}