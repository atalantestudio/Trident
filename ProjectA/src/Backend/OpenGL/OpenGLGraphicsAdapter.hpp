#pragma once

#include <glfw/glfw3.h>

#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLInstance.hpp"

#include "Engine/GraphicsAdapter.hpp"

namespace ProjectA {
	class OpenGLGraphicsAdapter : public GraphicsAdapter {
		public:
			OpenGLGraphicsAdapter(GLFWwindow* window);

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

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			GLFWwindow* window;
			const OpenGLInstance instance;
			const ref<OpenGLDevice> device;
	};
}