// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Base.hpp"
#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	static void loadProcedures() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			#define LOAD_PROCEDURE(procedure) procedure = GL::loadProcedure<decltype(procedure)>(#procedure)

			LOAD_PROCEDURE(glActiveTexture);
			LOAD_PROCEDURE(glAttachShader);
			LOAD_PROCEDURE(glBindBuffer);
			LOAD_PROCEDURE(glBindTexture);
			LOAD_PROCEDURE(glCreateProgram);
			LOAD_PROCEDURE(glCreateShader);
			LOAD_PROCEDURE(glCullFace);
			LOAD_PROCEDURE(glDeleteBuffers);
			LOAD_PROCEDURE(glDeleteProgram);
			LOAD_PROCEDURE(glDeleteShader);
			LOAD_PROCEDURE(glDeleteTextures);
			LOAD_PROCEDURE(glDepthFunc);
			LOAD_PROCEDURE(glDepthMask);
			LOAD_PROCEDURE(glDepthRange);
			LOAD_PROCEDURE(glDetachShader);
			LOAD_PROCEDURE(glDisable);
			LOAD_PROCEDURE(glDrawBuffer);
			LOAD_PROCEDURE(glDrawBuffers);
			LOAD_PROCEDURE(glEnable);
			LOAD_PROCEDURE(glEnableVertexAttribArray);
			LOAD_PROCEDURE(glFrontFace);
			LOAD_PROCEDURE(glFinish);
			LOAD_PROCEDURE(glGenBuffers);
			LOAD_PROCEDURE(glGenTextures);
			LOAD_PROCEDURE(glGetBufferParameteriv);
			LOAD_PROCEDURE(glGetFloatv);
			LOAD_PROCEDURE(glGetIntegerv);
			LOAD_PROCEDURE(glGetProgramInfoLog);
			LOAD_PROCEDURE(glGetProgramiv);
			LOAD_PROCEDURE(glGetShaderInfoLog);
			LOAD_PROCEDURE(glGetShaderiv);
			LOAD_PROCEDURE(glGetString);

			#if defined(BUILD_MODE_DEBUG)
				LOAD_PROCEDURE(glGetTexParameteriv);
			#endif

			LOAD_PROCEDURE(glLineWidth);
			LOAD_PROCEDURE(glLinkProgram);
			LOAD_PROCEDURE(glPixelStorei);
			LOAD_PROCEDURE(glPolygonMode);
			LOAD_PROCEDURE(glReadBuffer);
			LOAD_PROCEDURE(glScissor);
			LOAD_PROCEDURE(glTexParameteri);
			LOAD_PROCEDURE(glTexSubImage1D);
			LOAD_PROCEDURE(glTexSubImage2D);
			LOAD_PROCEDURE(glTexSubImage3D);
			LOAD_PROCEDURE(glUnmapBuffer);
			LOAD_PROCEDURE(glUseProgram);
			LOAD_PROCEDURE(glViewport);

			#if OPENGL_VERSION >= OPENGL_VERSION_3_0
				LOAD_PROCEDURE(glBindBufferRange);
				LOAD_PROCEDURE(glBindFramebuffer);
				LOAD_PROCEDURE(glBindVertexArray);
				LOAD_PROCEDURE(glBlitFramebuffer);

				#if defined(BUILD_MODE_DEBUG)
					LOAD_PROCEDURE(glCheckFramebufferStatus);
				#endif

				LOAD_PROCEDURE(glClearBufferfi);
				LOAD_PROCEDURE(glClearBufferfv);
				LOAD_PROCEDURE(glClearBufferiv);
				LOAD_PROCEDURE(glClearBufferuiv);
				LOAD_PROCEDURE(glDeleteFramebuffers);
				LOAD_PROCEDURE(glDeleteVertexArrays);
				LOAD_PROCEDURE(glDisablei);
				LOAD_PROCEDURE(glEnablei);
				LOAD_PROCEDURE(glFramebufferTexture2D);
				LOAD_PROCEDURE(glGenFramebuffers);
				LOAD_PROCEDURE(glGenVertexArrays);
				LOAD_PROCEDURE(glGetIntegeri_v);
				LOAD_PROCEDURE(glMapBufferRange);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_3_1
				LOAD_PROCEDURE(glCopyBufferSubData);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_3_2
				LOAD_PROCEDURE(glClientWaitSync);
				LOAD_PROCEDURE(glDeleteSync);
				LOAD_PROCEDURE(glFenceSync);
				LOAD_PROCEDURE(glGetInteger64v);
				LOAD_PROCEDURE(glProvokingVertex);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_3_3
				LOAD_PROCEDURE(glBindSampler);
				LOAD_PROCEDURE(glDeleteSamplers);
				LOAD_PROCEDURE(glGenSamplers);
				LOAD_PROCEDURE(glSamplerParameterf);
				LOAD_PROCEDURE(glSamplerParameterfv);
				LOAD_PROCEDURE(glSamplerParameteri);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_4_0
				LOAD_PROCEDURE(glBlendFunci);
				LOAD_PROCEDURE(glMinSampleShading);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_4_2
				LOAD_PROCEDURE(glBindImageTexture);
				LOAD_PROCEDURE(glMemoryBarrier);
				LOAD_PROCEDURE(glTexStorage2D);
				LOAD_PROCEDURE(glTexStorage3D);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_4_3
				LOAD_PROCEDURE(glBindVertexBuffer);

				#if defined(DEBUG_ENABLE_MESSENGER)
					LOAD_PROCEDURE(glDebugMessageCallback);
					LOAD_PROCEDURE(glDebugMessageInsert);
				#endif

				LOAD_PROCEDURE(glDispatchCompute);
				LOAD_PROCEDURE(glDispatchComputeIndirect);

				#if defined(BUILD_MODE_DEBUG)
					LOAD_PROCEDURE(glObjectLabel);
				#endif

				LOAD_PROCEDURE(glTexStorage2DMultisample);
				LOAD_PROCEDURE(glTexStorage3DMultisample);
				LOAD_PROCEDURE(glTextureView);
				LOAD_PROCEDURE(glVertexAttribBinding);
				LOAD_PROCEDURE(glVertexAttribFormat);
				LOAD_PROCEDURE(glVertexAttribIFormat);
				LOAD_PROCEDURE(glVertexAttribLFormat);
				LOAD_PROCEDURE(glVertexBindingDivisor);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_4_4
				LOAD_PROCEDURE(glBufferStorage);
			#endif

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				LOAD_PROCEDURE(glClipControl);
				LOAD_PROCEDURE(glGetTextureSubImage);
				LOAD_PROCEDURE(glVertexArrayElementBuffer);
			#endif

			#if defined(GL_SOURCE_FROM_SPIRV)
				#if OPENGL_VERSION >= OPENGL_VERSION_4_6
					LOAD_PROCEDURE(glShaderBinary);
					LOAD_PROCEDURE(glSpecializeShader);
				#endif
			#else
				LOAD_PROCEDURE(glCompileShader);
				LOAD_PROCEDURE(glShaderSource);
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

			#if OPENGL_VERSION >= OPENGL_VERSION_4_3
				LOAD_PROCEDURE(glMultiDrawArraysIndirect);
				LOAD_PROCEDURE(glMultiDrawElementsIndirect);
			#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
				LOAD_PROCEDURE(glDrawArraysIndirect);
				LOAD_PROCEDURE(glDrawElementsIndirect);
			#endif

			#undef LOAD_PROCEDURE
		#endif
	}

	#if defined(DEBUG_ENABLE_MESSENGER)
		static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
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

	GL::Surface::Surface(const Trident::Instance* instance) :
		Trident::Surface(instance)
	{}

	ref<Trident::Surface> GL::createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window) {
		static bool proceduresLoaded = false;

		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Surface> surface = allocateRef<GL::Surface>(instance);

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window->data);
			BOOL result;

			surface->deviceContext = windowData->hDC;

			PIXELFORMATDESCRIPTOR pixelFormatDescriptor{};
			pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
			pixelFormatDescriptor.nVersion = 1;
			pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDescriptor.cColorBits = 32;
			pixelFormatDescriptor.cRedBits = 0;
			pixelFormatDescriptor.cRedShift = 0;
			pixelFormatDescriptor.cGreenBits = 0;
			pixelFormatDescriptor.cGreenShift = 0;
			pixelFormatDescriptor.cBlueBits = 0;
			pixelFormatDescriptor.cBlueShift = 0;
			pixelFormatDescriptor.cAlphaBits = 0;
			pixelFormatDescriptor.cAlphaShift = 0;
			pixelFormatDescriptor.cAccumBits = 0;
			pixelFormatDescriptor.cAccumRedBits = 0;
			pixelFormatDescriptor.cAccumGreenBits = 0;
			pixelFormatDescriptor.cAccumBlueBits = 0;
			pixelFormatDescriptor.cAccumAlphaBits = 0;
			pixelFormatDescriptor.cDepthBits = 24;
			pixelFormatDescriptor.cStencilBits = 8;
			pixelFormatDescriptor.cAuxBuffers = 0;
			pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
			pixelFormatDescriptor.bReserved = 0;
			pixelFormatDescriptor.dwLayerMask = 0;
			pixelFormatDescriptor.dwVisibleMask = 0;
			pixelFormatDescriptor.dwDamageMask = 0;

			const sint32 pixelFormat = ChoosePixelFormat(surface->deviceContext, &pixelFormatDescriptor);

			WIN32_ASSERT(pixelFormat != 0);

			result = SetPixelFormat(surface->deviceContext, pixelFormat, &pixelFormatDescriptor);

			WIN32_ASSERT(result == TRUE);

			const GL::PFNWGLCREATECONTEXTPROC wglCreateContext = loadWglProcedure<GL::PFNWGLCREATECONTEXTPROC>("wglCreateContext");

			surface->renderingContext = wglCreateContext(surface->deviceContext);

			WIN32_ASSERT(surface->renderingContext != NULL);

			surface->wglMakeCurrent = loadWglProcedure<GL::PFNWGLMAKECURRENTPROC>("wglMakeCurrent");

			result = surface->wglMakeCurrent(surface->deviceContext, surface->renderingContext);

			WIN32_ASSERT(result == TRUE);

			const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = loadProcedure<PFNWGLCREATECONTEXTATTRIBSARBPROC>("wglCreateContextAttribsARB");

			result = surface->wglMakeCurrent(surface->deviceContext, NULL);

			WIN32_ASSERT(result == TRUE);

			surface->wglDeleteContext = loadWglProcedure<PFNWGLDELETECONTEXTPROC>("wglDeleteContext");

			result = surface->wglDeleteContext(surface->renderingContext);

			WIN32_ASSERT(result == TRUE);

			static constexpr sint32 majorVersion = OPENGL_VERSION / 100;
			static constexpr sint32 minorVersion = (OPENGL_VERSION - (OPENGL_VERSION / 100 * 100)) / 10;
			static constexpr sint32 layerPlane = 0;

			#if defined(DEBUG_ENABLE_MESSENGER)
				static constexpr sint32 flags = WGL_CONTEXT_DEBUG_BIT_ARB;
			#else
				static constexpr sint32 flags = 0;
			#endif

			static constexpr sint32 profileMask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

			static constexpr sint32 attributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
				WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
				WGL_CONTEXT_LAYER_PLANE_ARB, layerPlane,
				WGL_CONTEXT_FLAGS_ARB, flags,
				WGL_CONTEXT_PROFILE_MASK_ARB, profileMask,
				0,
			};

			surface->renderingContext = wglCreateContextAttribsARB(surface->deviceContext, NULL, attributes);

			WIN32_ASSERT(surface->renderingContext != NULL);

			result = surface->wglMakeCurrent(surface->deviceContext, surface->renderingContext);

			WIN32_ASSERT(result == TRUE);
		#endif

		if (!proceduresLoaded) {
			loadProcedures();

			proceduresLoaded = true;
		}

		#if defined(DEBUG_ENABLE_MESSENGER)
			#if OPENGL_VERSION >= OPENGL_VERSION_4_3
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(debugMessageCallback, nullptr);
				glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, 0, "Enabled OpenGL debug message callback.");
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		surface->format = ImageFormat::R8G8B8A8_UNORM;

		ref<GL::PhysicalDevice> _physicalDevice = static_pointer_cast<GL::PhysicalDevice>(physicalDevice);

		_physicalDevice->limits.bufferImageGranularity = 0;

		glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &_physicalDevice->limits.lineWidthGranularity);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, pointer_cast<GLint*>(&_physicalDevice->limits.maxColorAttachmentCount));

		_physicalDevice->limits.maxBoundDescriptorSetCount = max<uint32>();

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupCount[0]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupCount[1]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupCount[2]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupSize[0]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupSize[1]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, pointer_cast<GLint*>(&_physicalDevice->limits.maxComputeWorkgroupSize[2]));
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, pointer_cast<GLint*>(&_physicalDevice->limits.maxFramebufferSize[0]));
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, pointer_cast<GLint*>(&_physicalDevice->limits.maxFramebufferSize[1]));
		glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, pointer_cast<GLint*>(&_physicalDevice->limits.maxFramebufferLayerCount));
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, pointer_cast<GLint*>(&_physicalDevice->limits.maxImageDimension1D));
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, pointer_cast<GLint*>(&_physicalDevice->limits.maxImageDimension2D));
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, pointer_cast<GLint*>(&_physicalDevice->limits.maxImageDimension3D));
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, pointer_cast<GLint*>(&_physicalDevice->limits.maxImageLayerCount));

		_physicalDevice->limits.maxMemoryAllocationCount = max<uint32>();

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, pointer_cast<GLint*>(&_physicalDevice->limits.maxVertexInputAttributeCount));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, pointer_cast<GLint*>(&_physicalDevice->limits.maxVertexInputAttributeOffset));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, pointer_cast<GLint*>(&_physicalDevice->limits.maxVertexInputBindingCount));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, pointer_cast<GLint*>(&_physicalDevice->limits.maxVertexInputBindingStride));
		glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &_physicalDevice->limits.pointSizeGranularity);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glGetInteger64v(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, pointer_cast<GLint64*>(&_physicalDevice->limits.minStorageBufferOffsetAlignment));
			glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, pointer_cast<GLint64*>(&_physicalDevice->limits.minUniformBufferOffsetAlignment));
		#else
			GLint minStorageBufferOffsetAlignment;
			GLint minUniformBufferOffsetAlignment;

			glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &minStorageBufferOffsetAlignment);
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &minUniformBufferOffsetAlignment);

			_physicalDevice->limits.minStorageBufferOffsetAlignment = minStorageBufferOffsetAlignment;
			_physicalDevice->limits.minUniformBufferOffsetAlignment = minUniformBufferOffsetAlignment;
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_6
			GLint shaderBinaryFormatCount;

			glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &shaderBinaryFormatCount);

			// TODO(graphics/gl/spir-v)
			// glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS) can return 0 even if SPIR-V binaries are supported.
			// Tested on HP Victus laptop with Intel integrated GPU.
			if (shaderBinaryFormatCount > 0) {
				sequence<GLint> shaderBinaryFormats(shaderBinaryFormatCount);

				glGetIntegerv(GL_SHADER_BINARY_FORMATS, &shaderBinaryFormats[0]);

				GLint spirvShaderBinaryFormatIndex = 0;

				while (spirvShaderBinaryFormatIndex < shaderBinaryFormats.count()) {
					if (shaderBinaryFormats[spirvShaderBinaryFormatIndex] == GL_SHADER_BINARY_FORMAT_SPIR_V) {
						break;
					}

					spirvShaderBinaryFormatIndex += 1;
				}

				ADAPTER_ASSERT(spirvShaderBinaryFormatIndex < shaderBinaryFormatCount, "GL_SHADER_BINARY_FORMAT_SPIR_V is not supported.");
			}
		#endif

		return surface;
	}

	void GL::destroySurface(const Trident::Instance* instance, const Trident::Surface* surface) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			const GL::Surface* _surface = static_cast<const GL::Surface*>(surface);
			BOOL result;

			result = _surface->wglMakeCurrent(_surface->deviceContext, nullptr);

			WIN32_ASSERT(result == TRUE);

			result = _surface->wglDeleteContext(_surface->renderingContext);

			WIN32_ASSERT(result == TRUE);
		#endif
	}
}