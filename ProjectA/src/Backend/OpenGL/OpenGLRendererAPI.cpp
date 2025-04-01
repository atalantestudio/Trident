#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLRendererAPI.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"
#include "Backend/OpenGL/OpenGLSemaphore.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"
#include "Backend/OpenGL/OpenGLTexture.hpp"

namespace ProjectA {
	OpenGLRendererAPI::OpenGLRendererAPI(GLFWwindow* window) :
		window(window),
		instance(),
		device(createRef<OpenGLDevice>())
	{}

	ref<Device> OpenGLRendererAPI::getDevice() const {
		return device;
	}

	ref<Buffer> OpenGLRendererAPI::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<OpenGLBuffer>(descriptor);
	}

	ref<Fence> OpenGLRendererAPI::createFence() const {
		return createRef<OpenGLFence>();
	}

	ref<ComputePipeline> OpenGLRendererAPI::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		ENGINE_ASSERT(false, "TODO: OpenGLComputePipeline");

		return nullptr;
		//return createRef<>(descriptor);
	}

	ref<PipelineLayout> OpenGLRendererAPI::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<OpenGLPipelineLayout>(descriptor);
	}

	ref<RenderPass> OpenGLRendererAPI::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<OpenGLRenderPass>(descriptor);
	}

	ref<RenderPipeline> OpenGLRendererAPI::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<OpenGLRenderPipeline>(descriptor);
	}

	ref<Sampler> OpenGLRendererAPI::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<OpenGLSampler>(descriptor);
	}

	ref<Semaphore> OpenGLRendererAPI::createSemaphore() const {
		return createRef<OpenGLSemaphore>();
	}

	ref<ShaderModule> OpenGLRendererAPI::createShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code) const {
		return createRef<OpenGLShaderModule>(descriptor, code);
	}

	ref<Swapchain> OpenGLRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) {
		return createRef<OpenGLSwapchain>(descriptor);
	}

	ref<Texture> OpenGLRendererAPI::createTexture(const TextureDescriptor& descriptor) const {
		return createRef<OpenGLTexture>(descriptor);
	}
}