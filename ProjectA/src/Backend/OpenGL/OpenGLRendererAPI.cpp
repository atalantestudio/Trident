#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
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

	ref<CommandEncoder> OpenGLRendererAPI::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<OpenGLCommandEncoder>(descriptor);
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

	ref<ShaderModule> OpenGLRendererAPI::createShaderModule(const ShaderStage stage, const chain<uint8>& code) const {
		static constexpr GLenum shaderStages[] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
		};

		return createRef<OpenGLShaderModule>(shaderStages[static_cast<uint32>(stage)], code);
	}

	ref<Swapchain> OpenGLRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<OpenGLSwapchain>(window, descriptor);
	}

	ref<Texture> OpenGLRendererAPI::createTexture(const TextureDescriptor& descriptor) const {
		return createRef<OpenGLTexture>(descriptor);
	}
}