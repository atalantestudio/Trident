#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"
#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"
#include "Backend/OpenGL/OpenGLSemaphore.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(const Window& window) :
		deviceContext(window)
	{
		PROFILE_SCOPE(FUNC_SIG);

		deviceContext.setSwapInterval(1);

		LOAD_PROCEDURE_ADDRESS(deviceContext, glAttachShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindBuffer);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindBufferRange);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindFramebuffer);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindImageTexture);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindSampler);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindTextureUnit);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindVertexArray);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBindVertexBuffer);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glBlitFramebuffer);

		#ifdef BUILD_MODE_DEBUG
			LOAD_PROCEDURE_ADDRESS(deviceContext, glCheckFramebufferStatus);
		#endif

		LOAD_PROCEDURE_ADDRESS(deviceContext, glClientWaitSync);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCompileShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCopyNamedBufferSubData);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateBuffers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateFramebuffers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateProgram);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateSamplers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateTextures);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glCreateVertexArrays);

		#ifdef DEBUG_ENABLE_MESSENGER
			LOAD_PROCEDURE_ADDRESS(deviceContext, glDebugMessageCallback);
			LOAD_PROCEDURE_ADDRESS(deviceContext, glDebugMessageInsert);
		#endif

		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteBuffers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteFramebuffers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteProgram);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteSamplers);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteSync);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDeleteVertexArrays);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDetachShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDispatchCompute);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDrawArraysInstanced);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glDrawElementsInstanced);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glEnableVertexAttribArray);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glFenceSync);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glFramebufferTexture2D);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glGetIntegeri_v);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glGetProgramInfoLog);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glGetProgramiv);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glGetShaderInfoLog);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glGetShaderiv);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glLinkProgram);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glMapNamedBufferRange);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glMemoryBarrier);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glMinSampleShading);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glNamedBufferStorage);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glProvokingVertex);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glSamplerParameteri);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glShaderBinary);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glShaderSource);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glSpecializeShader);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glTextureStorage2D);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glTextureStorage2DMultisample);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glTextureSubImage2D);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glTextureView);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glUnmapNamedBuffer);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glUseProgram);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glVertexAttribBinding);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glVertexAttribFormat);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glVertexAttribIFormat);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glVertexAttribLFormat);
		LOAD_PROCEDURE_ADDRESS(deviceContext, glVertexBindingDivisor);

		#ifdef DEBUG_ENABLE_MESSENGER
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugMessageCallback, nullptr);

			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, 36, "Using OpenGL debug message callback");
		#endif

		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

		device = createRef<OpenGLDevice>(deviceContext);
	}

	ref<Buffer> OpenGLGraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<OpenGLBuffer>(descriptor);
	}

	ref<Fence> OpenGLGraphicsAdapter::createFence() const {
		return createRef<OpenGLFence>();
	}

	ref<Image> OpenGLGraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<OpenGLImage>(descriptor);
	}

	ref<ComputePipeline> OpenGLGraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<OpenGLComputePipeline>(descriptor);
	}

	ref<PipelineLayout> OpenGLGraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<OpenGLPipelineLayout>(descriptor);
	}

	ref<RenderPass> OpenGLGraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<OpenGLRenderPass>(descriptor);
	}

	ref<RenderPipeline> OpenGLGraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<OpenGLRenderPipeline>(descriptor);
	}

	ref<Sampler> OpenGLGraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<OpenGLSampler>(descriptor);
	}

	ref<Semaphore> OpenGLGraphicsAdapter::createSemaphore() const {
		return createRef<OpenGLSemaphore>();
	}

	ref<ShaderModule> OpenGLGraphicsAdapter::createShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<OpenGLShaderModule>(code, descriptor);
	}

	ref<Swapchain> OpenGLGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<OpenGLSwapchain>(descriptor);
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void OpenGLGraphicsAdapter::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			static constexpr std::string_view log = "[] []: [].";
			static constexpr char* sourceStrings[] = {
				"OpenGL API",
				"Window-system API",
				"Shader compiler",
				"Third-party",
				"Application",
				"Unknown source",
			};

			const char* sourceString = sourceStrings[source - GL_DEBUG_SOURCE_API];
			std::string_view typeString;

			switch (type) {
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					typeString = "deprecation warning";

					break;
				case GL_DEBUG_TYPE_ERROR:
					typeString = "error";

					break;
				case GL_DEBUG_TYPE_MARKER:
					typeString = "command stream annotation";

					break;
				case GL_DEBUG_TYPE_OTHER:
					typeString = "message";

					break;
				case GL_DEBUG_TYPE_PERFORMANCE:
					typeString = "performance warning";

					break;
				case GL_DEBUG_TYPE_POP_GROUP:
					typeString = "group popping";

					break;
				case GL_DEBUG_TYPE_PORTABILITY:
					typeString = "portability warning";

					break;
				case GL_DEBUG_TYPE_PUSH_GROUP:
					typeString = "group pushing";

					break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					typeString = "undefined behavior warning";

					break;
			}

			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					ENGINE_INFO(log, sourceString, typeString, message);

					break;
				case GL_DEBUG_SEVERITY_LOW:
				case GL_DEBUG_SEVERITY_MEDIUM:
					ENGINE_WARN(log, sourceString, typeString, message);

					break;
				case GL_DEBUG_SEVERITY_HIGH:
					ENGINE_ERROR(log, sourceString, typeString, message);

					break;
			}
		}
	#endif

	ref<Device> OpenGLGraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> OpenGLGraphicsAdapter::getSurface() const {
		// TODO
		DEBUG_ASSERT(false);

		return nullptr;
	}
}