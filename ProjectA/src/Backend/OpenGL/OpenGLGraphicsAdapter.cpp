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
	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window) :
		GraphicsAdapter(window),
		instance(window),
		surface(createRef<OpenGLSurface>())
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		instance.makeCurrent();

		loadProcedures();

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3 && defined(DEBUG_ENABLE_MESSENGER)
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugMessageCallback, nullptr);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, 0, "Enabled OpenGL debug message callback.");
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glNamedFramebufferDrawBuffers(0, 1, &defaultFramebufferDrawBuffer);
		#endif

		device = createRef<OpenGLDevice>(instance);
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
		return createRef<OpenGLSwapchain>(descriptor, instance, *surface);
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

	void OpenGLGraphicsAdapter::loadProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(instance.getProcedure(#procedure))

		LOAD_PROCEDURE(glAttachShader);
		LOAD_PROCEDURE(glBindBuffer);
		LOAD_PROCEDURE(glCreateProgram);
		LOAD_PROCEDURE(glCreateShader);
		LOAD_PROCEDURE(glCullFace);
		LOAD_PROCEDURE(glDeleteBuffers);
		LOAD_PROCEDURE(glDeleteProgram);
		LOAD_PROCEDURE(glDeleteShader);
		LOAD_PROCEDURE(glDeleteTextures);
		LOAD_PROCEDURE(glDepthFunc);
		LOAD_PROCEDURE(glDepthMask);
		LOAD_PROCEDURE(glDetachShader);
		LOAD_PROCEDURE(glDisable);
		LOAD_PROCEDURE(glEnable);
		LOAD_PROCEDURE(glFrontFace);
		LOAD_PROCEDURE(glGenTextures);
		LOAD_PROCEDURE(glGetFloatv);
		LOAD_PROCEDURE(glGetIntegerv);
		LOAD_PROCEDURE(glGetProgramInfoLog);
		LOAD_PROCEDURE(glGetProgramiv);
		LOAD_PROCEDURE(glGetShaderInfoLog);
		LOAD_PROCEDURE(glGetShaderiv);

		#if BUILD_MODE_DEBUG
			LOAD_PROCEDURE(glGetString);
		#endif

		LOAD_PROCEDURE(glLineWidth);
		LOAD_PROCEDURE(glLinkProgram);
		LOAD_PROCEDURE(glPixelStorei);
		LOAD_PROCEDURE(glPolygonMode);
		LOAD_PROCEDURE(glScissor);
		LOAD_PROCEDURE(glUseProgram);
		LOAD_PROCEDURE(glViewport);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			LOAD_PROCEDURE(glBindBufferRange);
			LOAD_PROCEDURE(glBindFramebuffer);
			LOAD_PROCEDURE(glBindVertexArray);
			LOAD_PROCEDURE(glDeleteFramebuffers);
			LOAD_PROCEDURE(glDeleteVertexArrays);
			LOAD_PROCEDURE(glDisablei);
			LOAD_PROCEDURE(glEnablei);
			LOAD_PROCEDURE(glGetIntegeri_v);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			LOAD_PROCEDURE(glClientWaitSync);
			LOAD_PROCEDURE(glDeleteSync);
			LOAD_PROCEDURE(glFenceSync);
			LOAD_PROCEDURE(glProvokingVertex);
			LOAD_PROCEDURE(glSamplerParameteri);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_3
			LOAD_PROCEDURE(glBindSampler);
			LOAD_PROCEDURE(glDeleteSamplers);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_0
			LOAD_PROCEDURE(glBlendFunci);
			LOAD_PROCEDURE(glMinSampleShading);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_2
			LOAD_PROCEDURE(glBindImageTexture);
			LOAD_PROCEDURE(glMemoryBarrier);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			#ifdef DEBUG_ENABLE_MESSENGER
				LOAD_PROCEDURE(glDebugMessageCallback);
				LOAD_PROCEDURE(glDebugMessageInsert);
			#endif

			LOAD_PROCEDURE(glDispatchCompute);
			LOAD_PROCEDURE(glDispatchComputeIndirect);
			LOAD_PROCEDURE(glTextureView);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glGetTextureSubImage);
			LOAD_PROCEDURE(glVertexArrayElementBuffer);
		#endif

		#ifdef OPENGL_USE_SPIRV
			#if OPENGL_VERSION >= OPENGL_VERSION_4_6
				LOAD_PROCEDURE(glShaderBinary);
				LOAD_PROCEDURE(glSpecializeShader);
			#endif
		#else
			#if OPENGL_VERSION >= OPENGL_VERSION_2_0
				LOAD_PROCEDURE(glCompileShader);
				LOAD_PROCEDURE(glShaderSource);
			#endif
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glCopyNamedBufferSubData);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
			LOAD_PROCEDURE(glCopyBufferSubData);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glCreateBuffers);
			LOAD_PROCEDURE(glGetNamedBufferParameteriv);
			LOAD_PROCEDURE(glUnmapNamedBuffer);
		#else
			LOAD_PROCEDURE(glGenBuffers);
			LOAD_PROCEDURE(glGetBufferParameteriv);
			LOAD_PROCEDURE(glUnmapBuffer);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glBindTextureUnit);
			LOAD_PROCEDURE(glCreateTextures);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glGetTextureParameteriv);
			#endif

			LOAD_PROCEDURE(glTextureSubImage1D);
			LOAD_PROCEDURE(glTextureSubImage2D);
			LOAD_PROCEDURE(glTextureSubImage3D);
		#else
			LOAD_PROCEDURE(glActiveTexture);
			LOAD_PROCEDURE(glBindTexture);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glGetTexParameteriv);
			#endif

			LOAD_PROCEDURE(glTexSubImage1D);
			LOAD_PROCEDURE(glTexSubImage2D);
			LOAD_PROCEDURE(glTexSubImage3D);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glBlitNamedFramebuffer);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glCheckNamedFramebufferStatus);
			#endif

			LOAD_PROCEDURE(glClearNamedFramebufferfi);
			LOAD_PROCEDURE(glClearNamedFramebufferfv);
			LOAD_PROCEDURE(glCreateFramebuffers);
			LOAD_PROCEDURE(glNamedFramebufferTexture);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			LOAD_PROCEDURE(glBlitFramebuffer);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glCheckFramebufferStatus);
			#endif

			LOAD_PROCEDURE(glClearBufferfi);
			LOAD_PROCEDURE(glClearBufferfv);
			LOAD_PROCEDURE(glFramebufferTexture2D);
			LOAD_PROCEDURE(glGenFramebuffers);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glCreateSamplers);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_3
			LOAD_PROCEDURE(glGenSamplers);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glCreateVertexArrays);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			LOAD_PROCEDURE(glGenVertexArrays);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_2
			LOAD_PROCEDURE(glDrawArraysInstancedBaseInstance);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
			LOAD_PROCEDURE(glDrawArraysInstanced);
		#else
			LOAD_PROCEDURE(glDrawArrays);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_2
			LOAD_PROCEDURE(glDrawElementsInstancedBaseVertexBaseInstance);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_2
			LOAD_PROCEDURE(glDrawElementsInstancedBaseVertex);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
			LOAD_PROCEDURE(glDrawElementsInstanced);
		#else
			LOAD_PROCEDURE(glDrawElements);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glEnableVertexArrayAttrib);
		#else
			LOAD_PROCEDURE(glEnableVertexAttribArray);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glMapNamedBufferRange);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			LOAD_PROCEDURE(glMapBufferRange);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			LOAD_PROCEDURE(glMultiDrawArraysIndirect);
			LOAD_PROCEDURE(glMultiDrawElementsIndirect);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
			LOAD_PROCEDURE(glDrawArraysIndirect);
			LOAD_PROCEDURE(glDrawElementsIndirect);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glNamedBufferStorage);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_4
			LOAD_PROCEDURE(glBufferStorage);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glNamedFramebufferDrawBuffers);
			LOAD_PROCEDURE(glNamedFramebufferReadBuffer);
		#else
			LOAD_PROCEDURE(glDrawBuffer);
			LOAD_PROCEDURE(glDrawBuffers);
			LOAD_PROCEDURE(glReadBuffer);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glTextureStorage2D);
			LOAD_PROCEDURE(glTextureStorage3D);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_2
			LOAD_PROCEDURE(glTexStorage2D);
			LOAD_PROCEDURE(glTexStorage3D);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glTextureStorage2DMultisample);
			LOAD_PROCEDURE(glTextureStorage3DMultisample);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
			LOAD_PROCEDURE(glTexStorage2DMultisample);
			LOAD_PROCEDURE(glTexStorage3DMultisample);
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			LOAD_PROCEDURE(glVertexArrayAttribBinding);
			LOAD_PROCEDURE(glVertexArrayAttribFormat);
			LOAD_PROCEDURE(glVertexArrayAttribIFormat);
			LOAD_PROCEDURE(glVertexArrayAttribLFormat);
			LOAD_PROCEDURE(glVertexArrayBindingDivisor);
			LOAD_PROCEDURE(glVertexArrayVertexBuffer);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
			LOAD_PROCEDURE(glBindVertexBuffer);
			LOAD_PROCEDURE(glVertexAttribBinding);
			LOAD_PROCEDURE(glVertexAttribFormat);
			LOAD_PROCEDURE(glVertexAttribIFormat);
			LOAD_PROCEDURE(glVertexAttribLFormat);
			LOAD_PROCEDURE(glVertexBindingDivisor);
		#endif

		#undef LOAD_PROCEDURE
	}
}