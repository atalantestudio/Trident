#include "pch.hpp"

#include "Platform/OpenGL/OpenGLBuffer.hpp"
#include "Platform/OpenGL/OpenGLCommandEncoder.hpp"
#include "Platform/OpenGL/OpenGLFence.hpp"
#include "Platform/OpenGL/OpenGLPipelineLayout.hpp"
#include "Platform/OpenGL/OpenGLRendererAPI.hpp"
#include "Platform/OpenGL/OpenGLRenderPass.hpp"
#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLSemaphore.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"
#include "Platform/OpenGL/OpenGLSwapchain.hpp"

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

	ref<CommandEncoder> OpenGLRendererAPI::createCommandEncoder() const {
		return createRef<OpenGLCommandEncoder>();
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

	ref<Semaphore> OpenGLRendererAPI::createSemaphore() const {
		return createRef<OpenGLSemaphore>();
	}

	ref<ShaderModule> OpenGLRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr GLenum shaderStages[] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
		};

		return createRef<OpenGLShaderModule>(shaderStages[static_cast<uint32>(stage)], static_cast<const GLchar* const>(code.c_str()));
	}

	ref<Swapchain> OpenGLRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<OpenGLSwapchain>(window, descriptor);
	}
}