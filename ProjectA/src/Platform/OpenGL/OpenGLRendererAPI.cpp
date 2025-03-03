#include "pch.hpp"

#include "Platform/OpenGL/OpenGLBuffer.hpp"
#include "Platform/OpenGL/OpenGLCommandEncoder.hpp"
#include "Platform/OpenGL/OpenGLPipelineLayout.hpp"
#include "Platform/OpenGL/OpenGLRendererAPI.hpp"
#include "Platform/OpenGL/OpenGLRenderPass.hpp"
#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"
#include "Platform/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLRendererAPI::OpenGLRendererAPI(GLFWwindow* window) :
		window(window),
		instance(),
		device(createRef<OpenGLDevice>())
	{}

	/*void OpenGLRendererAPI::setViewport(int32 x, int32 y, uint32 width, uint32 height) const {
		glViewport(x, y, width, height);
	}*/

	/*void OpenGLRendererAPI::setClearColor(const vec4<float32>& color) const {
		glClearColor(color[0], color[1], color[2], color[3]);
	}*/

	/*void OpenGLRendererAPI::clear() const {
		PROFILE_SCOPE(FUNC_SIG);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}*/

	/*void OpenGLRendererAPI::drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const {
		PROFILE_SCOPE(FUNC_SIG);

		if (indexCount == 0) {
			indexCount = renderPipeline->getIndexBuffer()->getCount();
		}

		const GLenum primitiveTopology = ((OpenGLRenderPipeline*) renderPipeline.get())->primitiveTopology;

		glDrawElements(primitiveTopology, indexCount, GL_UNSIGNED_INT, nullptr);
	}*/

	ref<Device> OpenGLRendererAPI::getDevice() const {
		return device;
	}

	ref<Buffer> OpenGLRendererAPI::createBuffer(const BufferDescriptor& descriptor) {
		return createRef<OpenGLBuffer>(descriptor);
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