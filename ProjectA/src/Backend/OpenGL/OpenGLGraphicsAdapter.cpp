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

		glAttachShader = static_cast<PFNGLATTACHSHADERPROC>(deviceContext.getProcedureAddress("glAttachShader"));
		glBindBuffer = static_cast<PFNGLBINDBUFFERPROC>(deviceContext.getProcedureAddress("glBindBuffer"));
		glBindBufferRange = static_cast<PFNGLBINDBUFFERRANGEPROC>(deviceContext.getProcedureAddress("glBindBufferRange"));
		glBindImageTexture = static_cast<PFNGLBINDIMAGETEXTUREPROC>(deviceContext.getProcedureAddress("glBindImageTexture"));
		glBindSampler = static_cast<PFNGLBINDSAMPLERPROC>(deviceContext.getProcedureAddress("glBindSampler"));
		glBindTextureUnit = static_cast<PFNGLBINDTEXTUREUNITPROC>(deviceContext.getProcedureAddress("glBindTextureUnit"));
		glBindVertexArray = static_cast<PFNGLBINDVERTEXARRAYPROC>(deviceContext.getProcedureAddress("glBindVertexArray"));
		glBindVertexBuffer = static_cast<PFNGLBINDVERTEXBUFFERPROC>(deviceContext.getProcedureAddress("glBindVertexBuffer"));
		glClear = static_cast<PFNGLCLEARPROC>(deviceContext.getProcedureAddress("glClear"));
		glClientWaitSync = static_cast<PFNGLCLIENTWAITSYNCPROC>(deviceContext.getProcedureAddress("glClientWaitSync"));
		glCopyNamedBufferSubData = static_cast<PFNGLCOPYNAMEDBUFFERSUBDATAPROC>(deviceContext.getProcedureAddress("glCopyNamedBufferSubData"));
		glCreateBuffers = static_cast<PFNGLCREATEBUFFERSPROC>(deviceContext.getProcedureAddress("glCreateBuffers"));
		glCreateProgram = static_cast<PFNGLCREATEPROGRAMPROC>(deviceContext.getProcedureAddress("glCreateProgram"));
		glCreateSamplers = static_cast<PFNGLCREATESAMPLERSPROC>(deviceContext.getProcedureAddress("glCreateSamplers"));
		glCreateShader = static_cast<PFNGLCREATESHADERPROC>(deviceContext.getProcedureAddress("glCreateShader"));
		glCreateTextures = static_cast<PFNGLCREATETEXTURESPROC>(deviceContext.getProcedureAddress("glCreateTextures"));
		glCreateVertexArrays = static_cast<PFNGLCREATEVERTEXARRAYSPROC>(deviceContext.getProcedureAddress("glCreateVertexArrays"));

		#ifdef DEBUG_ENABLE_MESSENGER
			glDebugMessageCallback = static_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(deviceContext.getProcedureAddress("glDebugMessageCallback"));
			glDebugMessageInsert = static_cast<PFNGLDEBUGMESSAGEINSERTPROC>(deviceContext.getProcedureAddress("glDebugMessageInsert"));
		#endif

		glDeleteBuffers = static_cast<PFNGLDELETEBUFFERSPROC>(deviceContext.getProcedureAddress("glDeleteBuffers"));
		glDeleteProgram = static_cast<PFNGLDELETEPROGRAMPROC>(deviceContext.getProcedureAddress("glDeleteProgram"));
		glDeleteSamplers = static_cast<PFNGLDELETESAMPLERSPROC>(deviceContext.getProcedureAddress("glDeleteSamplers"));
		glDeleteShader = static_cast<PFNGLDELETESHADERPROC>(deviceContext.getProcedureAddress("glDeleteShader"));
		glDeleteSync = static_cast<PFNGLDELETESYNCPROC>(deviceContext.getProcedureAddress("glDeleteSync"));
		glDeleteVertexArrays = static_cast<PFNGLDELETEVERTEXARRAYSPROC>(deviceContext.getProcedureAddress("glDeleteVertexArrays"));
		glDetachShader = static_cast<PFNGLDETACHSHADERPROC>(deviceContext.getProcedureAddress("glDetachShader"));
		glDispatchCompute = static_cast<PFNGLDISPATCHCOMPUTEPROC>(deviceContext.getProcedureAddress("glDispatchCompute"));
		glDrawArraysInstanced = static_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(deviceContext.getProcedureAddress("glDrawArraysInstanced"));
		glDrawElementsInstanced = static_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(deviceContext.getProcedureAddress("glDrawElementsInstanced"));
		glEnable = static_cast<PFNGLENABLEPROC>(deviceContext.getProcedureAddress("glEnable"));
		glEnableVertexAttribArray = static_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(deviceContext.getProcedureAddress("glEnableVertexAttribArray"));
		glFenceSync = static_cast<PFNGLFENCESYNCPROC>(deviceContext.getProcedureAddress("glFenceSync"));
		glGetIntegeri_v = static_cast<PFNGLGETINTEGERI_VPROC>(deviceContext.getProcedureAddress("glGetIntegeri_v"));
		glGetProgramInfoLog = static_cast<PFNGLGETPROGRAMINFOLOGPROC>(deviceContext.getProcedureAddress("glGetProgramInfoLog"));
		glGetProgramiv = static_cast<PFNGLGETPROGRAMIVPROC>(deviceContext.getProcedureAddress("glGetProgramiv"));
		glGetShaderInfoLog = static_cast<PFNGLGETSHADERINFOLOGPROC>(deviceContext.getProcedureAddress("glGetShaderInfoLog"));
		glGetShaderiv = static_cast<PFNGLGETSHADERIVPROC>(deviceContext.getProcedureAddress("glGetShaderiv"));
		glLinkProgram = static_cast<PFNGLLINKPROGRAMPROC>(deviceContext.getProcedureAddress("glLinkProgram"));
		glMapNamedBufferRange = static_cast<PFNGLMAPNAMEDBUFFERRANGEPROC>(deviceContext.getProcedureAddress("glMapNamedBufferRange"));
		glMemoryBarrier = static_cast<PFNGLMEMORYBARRIERPROC>(deviceContext.getProcedureAddress("glMemoryBarrier"));
		glNamedBufferStorage = static_cast<PFNGLNAMEDBUFFERSTORAGEPROC>(deviceContext.getProcedureAddress("glNamedBufferStorage"));
		glSamplerParameteri = static_cast<PFNGLSAMPLERPARAMETERIPROC>(deviceContext.getProcedureAddress("glSamplerParameteri"));
		glShaderBinary = static_cast<PFNGLSHADERBINARYPROC>(deviceContext.getProcedureAddress("glShaderBinary"));
		glSpecializeShader = static_cast<PFNGLSPECIALIZESHADERPROC>(deviceContext.getProcedureAddress("glSpecializeShader"));
		glTextureStorage2D = static_cast<PFNGLTEXTURESTORAGE2DPROC>(deviceContext.getProcedureAddress("glTextureStorage2D"));
		glTextureSubImage2D = static_cast<PFNGLTEXTURESUBIMAGE2DPROC>(deviceContext.getProcedureAddress("glTextureSubImage2D"));
		glTextureView = static_cast<PFNGLTEXTUREVIEWPROC>(deviceContext.getProcedureAddress("glTextureView"));
		glUnmapNamedBuffer = static_cast<PFNGLUNMAPNAMEDBUFFERPROC>(deviceContext.getProcedureAddress("glUnmapNamedBuffer"));
		glUseProgram = static_cast<PFNGLUSEPROGRAMPROC>(deviceContext.getProcedureAddress("glUseProgram"));
		glVertexAttribBinding = static_cast<PFNGLVERTEXATTRIBBINDINGPROC>(deviceContext.getProcedureAddress("glVertexAttribBinding"));
		glVertexAttribFormat = static_cast<PFNGLVERTEXATTRIBFORMATPROC>(deviceContext.getProcedureAddress("glVertexAttribFormat"));
		glVertexAttribIFormat = static_cast<PFNGLVERTEXATTRIBIFORMATPROC>(deviceContext.getProcedureAddress("glVertexAttribIFormat"));
		glVertexAttribLFormat = static_cast<PFNGLVERTEXATTRIBLFORMATPROC>(deviceContext.getProcedureAddress("glVertexAttribLFormat"));
		glVertexBindingDivisor = static_cast<PFNGLVERTEXBINDINGDIVISORPROC>(deviceContext.getProcedureAddress("glVertexBindingDivisor"));
		glViewport = static_cast<PFNGLVIEWPORTPROC>(deviceContext.getProcedureAddress("glViewport"));

		#ifdef DEBUG_ENABLE_MESSENGER
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugMessageCallback, nullptr);

			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, 36, "Using OpenGL debug message callback");
		#endif

		device = createRef<OpenGLDevice>(deviceContext);
	}

	ref<Device> OpenGLGraphicsAdapter::getDevice() const {
		return device;
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
}