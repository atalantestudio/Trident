#pragma once

#include "Backend/OpenGL/OpenGLDevice.hpp"

#include "Engine/API/DeviceContext.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/GraphicsAdapter.hpp"

namespace ProjectA {
	class OpenGLGraphicsAdapter : public GraphicsAdapter {
		public:
			OpenGLGraphicsAdapter(const Window& window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const override;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const override;

			ref<Fence> createFence() const override;

			ref<Image> createImage(const ImageDescriptor& descriptor) const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const override;

			ref<Semaphore> createSemaphore() const override;

			ref<ShaderModule> createShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			#ifdef DEBUG_ENABLE_MESSENGER
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif

		private:
			const Window& window;
			const DeviceContext deviceContext;
			ref<OpenGLDevice> device;
	};
}