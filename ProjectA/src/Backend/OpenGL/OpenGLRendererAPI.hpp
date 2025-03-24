#pragma once

#include <glfw/glfw3.h>

#include "Engine/Renderer/RendererAPI.hpp"

#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLInstance.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			OpenGLRendererAPI(GLFWwindow* window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const override;

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			ref<Fence> createFence() const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const override;

			ref<Semaphore> createSemaphore() const override;

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const chain<uint8>& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) override;

			ref<Texture> createTexture(const TextureDescriptor& descriptor) const override;

		private:
			GLFWwindow* window;
			const OpenGLInstance instance;
			ref<OpenGLDevice> device;
	};
}