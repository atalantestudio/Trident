#pragma once

#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLSurface.hpp"

#include "Engine/API/Window/Window.hpp"
#include "Engine/GraphicsAdapter.hpp"

namespace ProjectA {
	class OpenGLGraphicsAdapter final : public GraphicsAdapter {
		public:
			explicit OpenGLGraphicsAdapter(const Window& window);

			~OpenGLGraphicsAdapter() final override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<Fence> createFence() const final override;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const final override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Semaphore> createSemaphore() const final override;

			ref<ShaderModule> createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const final override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<Device> getDevice() const final override;

			ref<Surface> getSurface() const final override;

		private:
			#ifdef DEBUG_ENABLE_MESSENGER
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif

		private:
			const OpenGLRenderingContext renderingContext;
			ref<OpenGLDevice> device;
			ref<OpenGLSurface> surface;
	};
}