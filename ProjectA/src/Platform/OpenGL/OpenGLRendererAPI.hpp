#pragma once

#ifdef BUILD_MODE_DEBUG
	#include <glad/glad.h>
#endif

#include "Engine/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			OpenGLRendererAPI();

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) override;

			ref<CommandBuffer> createCommandBuffer() const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			#ifdef BUILD_MODE_DEBUG
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}