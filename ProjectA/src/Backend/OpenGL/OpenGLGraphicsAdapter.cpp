#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"
#include "Backend/OpenGL/OpenGLFramebuffer.hpp"
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
	void OpenGLGraphicsAdapter::loadProcedures(const OpenGLRenderingContext& renderingContext) {
		PROFILE_SCOPE(FUNC_SIG);

		#define LOAD_PROCEDURE_ADDRESS(renderingContext, procedure) procedure = static_cast<decltype(procedure)>(renderingContext.getProcedureAddress(#procedure))

		LOAD_PROCEDURE_ADDRESS(renderingContext, glAttachShader);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindBuffer);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindBufferRange);

		// TODO: glBindFramebuffer should only be loaded in a non-DSA context.
		//       See OpenGLEndRenderPassCommand.
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindFramebuffer);

		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindImageTexture);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindSampler);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBindVertexArray);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glBlendFunci);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glClientWaitSync);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateProgram);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateShader);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteBuffers);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteFramebuffers);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteProgram);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteSamplers);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteShader);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteSync);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDeleteVertexArrays);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDetachShader);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDisablei);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glDispatchCompute);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glEnablei);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glEnableVertexAttribArray);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glFenceSync);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetIntegeri_v);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetProgramInfoLog);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetProgramiv);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetShaderInfoLog);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetShaderiv);

		// TODO: There is no non-DSA variant for this function.
		LOAD_PROCEDURE_ADDRESS(renderingContext, glGetTextureSubImage);

		LOAD_PROCEDURE_ADDRESS(renderingContext, glLinkProgram);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glMemoryBarrier);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glMinSampleShading);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glProvokingVertex);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glSamplerParameteri);

		// TODO: There is no non-DSA variant for this function.
		LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureView);

		LOAD_PROCEDURE_ADDRESS(renderingContext, glUseProgram);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexAttribBinding);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexAttribFormat);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexAttribIFormat);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexAttribLFormat);
		LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexBindingDivisor);

		#ifdef DEBUG_ENABLE_MESSENGER
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDebugMessageCallback);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDebugMessageInsert);
		#endif

		#ifdef OPENGL_USE_DSA
			LOAD_PROCEDURE_ADDRESS(renderingContext, glBindTextureUnit);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glBlitNamedFramebuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glClearNamedFramebufferfi);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glClearNamedFramebufferfv);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCopyNamedBufferSubData);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateBuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateFramebuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateSamplers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateTextures);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCreateVertexArrays);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glMapNamedBufferRange);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glNamedBufferStorage);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glNamedFramebufferDrawBuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glNamedFramebufferReadBuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glNamedFramebufferTexture);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureStorage2D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureStorage2DMultisample);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureStorage3D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureStorage3DMultisample);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureSubImage1D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureSubImage2D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTextureSubImage3D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glUnmapNamedBuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexArrayElementBuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glVertexArrayVertexBuffer);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE_ADDRESS(renderingContext, glCheckNamedFramebufferStatus);
			#endif
		#else
			LOAD_PROCEDURE_ADDRESS(renderingContext, glActiveTexture);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glBindVertexBuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glBlitFramebuffer);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glBufferStorage);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glClearBufferfi);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glClearBufferfv);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCopyBufferSubData);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawBuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glFramebufferTexture2D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glGenBuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glGenFramebuffers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glGenSamplers);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glGenVertexArrays);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glMapBufferRange);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexStorage2D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexStorage2DMultisample);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexStorage3D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexStorage3DMultisample);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexSubImage1D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexSubImage2D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glTexSubImage3D);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glUnmapBuffer);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE_ADDRESS(renderingContext, glCheckFramebufferStatus);
			#endif
		#endif

		#ifdef OPENGL_USE_SPIRV
			LOAD_PROCEDURE_ADDRESS(renderingContext, glShaderBinary);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glSpecializeShader);
		#else
			LOAD_PROCEDURE_ADDRESS(renderingContext, glCompileShader);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glShaderSource);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_2
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawArraysInstancedBaseInstance);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawElementsInstancedBaseVertexBaseInstance);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_2
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawElementsInstancedBaseVertex);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawArraysInstanced);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawElementsInstanced);
		#elif OPENGL_VERSION >= OPENGL_VERSION_2_0
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawArrays);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawElements);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			LOAD_PROCEDURE_ADDRESS(renderingContext, glMultiDrawArraysIndirect);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glMultiDrawElementsIndirect);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawArraysIndirect);
			LOAD_PROCEDURE_ADDRESS(renderingContext, glDrawElementsIndirect);
		#endif

		#undef LOAD_PROCEDURE_ADDRESS
	}

	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(const Window& window) :
		renderingContext(window),
		surface(createRef<OpenGLSurface>())
	{
		PROFILE_SCOPE(FUNC_SIG);

		renderingContext.makeCurrent();

		loadProcedures(renderingContext);

		#ifdef DEBUG_ENABLE_MESSENGER
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugMessageCallback, nullptr);

			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, 0, "Enabled OpenGL debug message callback.");
		#endif

		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

		#ifdef OPENGL_USE_DSA
			glNamedFramebufferDrawBuffers(0, 1, &defaultFramebufferDrawBuffer);
		#endif

		device = createRef<OpenGLDevice>(renderingContext);
	}

	OpenGLGraphicsAdapter::~OpenGLGraphicsAdapter() {}

	ref<Buffer> OpenGLGraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<OpenGLBuffer>(descriptor);
	}

	ref<Fence> OpenGLGraphicsAdapter::createFence() const {
		return createRef<OpenGLFence>();
	}

	ref<Framebuffer> OpenGLGraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return createRef<OpenGLFramebuffer>(descriptor);
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

	ref<ShaderModule> OpenGLGraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<OpenGLShaderModule>(code, descriptor);
	}

	ref<Swapchain> OpenGLGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<OpenGLSwapchain>(descriptor, renderingContext, *surface);
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void OpenGLGraphicsAdapter::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			static constexpr std::string_view log = "[[] []] []";
			static constexpr char8* sourceStrings[] = {
				"OpenGL API",
				"Window-system API",
				"Shader compiler",
				"Third-party",
				"Application",
				"Unknown source",
			};

			const char8* sourceString = sourceStrings[source - GL_DEBUG_SOURCE_API];

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
		return surface;
	}
}