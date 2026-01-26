// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Sampler.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/GL/Swapchain.hpp"

namespace ProjectA {
	GL::GraphicsAdapter::GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties) :
		ProjectA::GraphicsAdapter(window)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		logger = ConsoleLogger(std::cout, LogLevel::ALL, "OPENGL");

		if (properties.PreferIntegratedGPU) {
			logger.warning("GPU selection can only be done at compile-time. Please enable the GL_PREFER_INTEGRATED_GPU macro instead.");
		}

		instance = createRef<GL::Instance>(window);
		instance->makeCurrent();

		surface = createRef<GL::Surface>();

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

		device = createRef<GL::Device>(*instance);
	}

	ref<Buffer> GL::GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<GL::Buffer>(descriptor);
	}

	ref<Fence> GL::GraphicsAdapter::createFence(const FenceDescriptor& descriptor) const {
		return createRef<GL::Fence>(descriptor);
	}

	ref<Framebuffer> GL::GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return createRef<GL::Framebuffer>(descriptor);
	}

	ref<Image> GL::GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<GL::Image>(descriptor);
	}

	ref<ComputePipeline> GL::GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<GL::ComputePipeline>(descriptor);
	}

	ref<DescriptorSetLayout> GL::GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		return createRef<DescriptorSetLayout>(bindings);
	}

	ref<PipelineLayout> GL::GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<PipelineLayout>(descriptor);
	}

	ref<RenderPass> GL::GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<GL::RenderPass>(descriptor);
	}

	ref<RenderPipeline> GL::GraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<GL::RenderPipeline>(descriptor);
	}

	ref<Sampler> GL::GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<GL::Sampler>(descriptor);
	}

	ref<Semaphore> GL::GraphicsAdapter::createSemaphore() const {
		return createRef<Semaphore>();
	}

	ref<ShaderModule> GL::GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		return createRef<GL::ShaderModule>(descriptor, asset);
	}

	ref<ShaderModule> GL::GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		return createRef<GL::ShaderModule>(descriptor, code);
	}

	ref<Swapchain> GL::GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<GL::Swapchain>(descriptor, *instance, *surface);
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void GL::GraphicsAdapter::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION: {
					ADAPTER_INFO(message);

					break;
				}
				case GL_DEBUG_SEVERITY_LOW:
				case GL_DEBUG_SEVERITY_MEDIUM: {
					ADAPTER_WARNING(message);

					break;
				}
				case GL_DEBUG_SEVERITY_HIGH: {
					ADAPTER_ERROR(message);

					break;
				}
			}
		}
	#endif

	ref<Device> GL::GraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> GL::GraphicsAdapter::getSurface() const {
		return surface;
	}

	void GL::GraphicsAdapter::loadProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(instance->getProcedure(#procedure))

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
		LOAD_PROCEDURE(glDrawBuffers);
		LOAD_PROCEDURE(glEnable);
		LOAD_PROCEDURE(glFrontFace);
		LOAD_PROCEDURE(glFinish);
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

		#ifdef GL_SOURCE_FROM_SPIRV
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

			LOAD_PROCEDURE(glTextureParameteri);
			LOAD_PROCEDURE(glTextureSubImage1D);
			LOAD_PROCEDURE(glTextureSubImage2D);
			LOAD_PROCEDURE(glTextureSubImage3D);
		#else
			LOAD_PROCEDURE(glActiveTexture);
			LOAD_PROCEDURE(glBindTexture);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glGetTexParameteriv);
			#endif

			LOAD_PROCEDURE(glTexParameteri);
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
			LOAD_PROCEDURE(glClearNamedFramebufferiv);
			LOAD_PROCEDURE(glClearNamedFramebufferuiv);
			LOAD_PROCEDURE(glCreateFramebuffers);
			LOAD_PROCEDURE(glNamedFramebufferTexture);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			LOAD_PROCEDURE(glBlitFramebuffer);

			#ifdef BUILD_MODE_DEBUG
				LOAD_PROCEDURE(glCheckFramebufferStatus);
			#endif

			LOAD_PROCEDURE(glClearBufferfi);
			LOAD_PROCEDURE(glClearBufferfv);
			LOAD_PROCEDURE(glClearBufferiv);
			LOAD_PROCEDURE(glClearBufferuiv);
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
			LOAD_PROCEDURE(glNamedFramebufferReadBuffer);
		#else
			LOAD_PROCEDURE(glDrawBuffer);
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