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

			void bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) override;

			void bindVertexBuffer(const ref<Buffer>& vertexBuffer, const uint32 stride) override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) override;

			void setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) override;

			void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) override;

			void drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		private:
			std::vector<ref<OpenGLCommand>> commandBuffer;
			const OpenGLRenderPipeline* renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;
	};
}