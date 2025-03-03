#pragma once

#include <glfw/glfw3.h>

#include "Engine/Renderer/RendererAPI.hpp"

#include "Platform/OpenGL/OpenGLDevice.hpp"
#include "Platform/OpenGL/OpenGLInstance.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			OpenGLRendererAPI(GLFWwindow* window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) override;

			ref<CommandEncoder> createCommandEncoder() const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			GLFWwindow* window;
			const OpenGLInstance instance;
			ref<OpenGLDevice> device;
	};
}