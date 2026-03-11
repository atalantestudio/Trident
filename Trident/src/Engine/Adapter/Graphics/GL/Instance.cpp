// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Window/Win32/base.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	void* GL::Instance::getWglProcedure(view<char8> procedureName) {
		static const PlatformLibrary opengl32Library = Platform::openLibrary("opengl32.dll");

		return opengl32Library.getProcedure(procedureName);
	}

	GL::Instance::Instance(const Trident::GraphicsAdapter* adapter) :
		Trident::Instance(adapter)
	{}

	void* GL::Instance::getProcedure(view<char8> procedureName) const {
		static const PFNWGLGETPROCADDRESSPROC wglGetProcAddress = static_cast<PFNWGLGETPROCADDRESSPROC>(getWglProcedure("wglGetProcAddress"));

		const PROC procedure = wglGetProcAddress(&procedureName[0]);

		// See https://wikis.khronos.org/opengl/Load_OpenGL_Functions.
		if (procedure == nullptr || procedure == (void*) -1 || procedure == (void*) 1 || procedure == (void*) 2 || procedure == (void*) 3) {
			return getWglProcedure(procedureName);
		}

		return procedure;
	}

	// TODO: Move in GL::Swapchain::update.
	/* void GL::Instance::setSwapInterval(sint32 interval) const {
		BOOL result;

		static const PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = static_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(getProcedure("wglGetExtensionsStringEXT"));
		static const std::string extensionsString = wglGetExtensionsStringEXT();

		ADAPTER_ASSERT(extensionsString.find("WGL_EXT_swap_control") != std::string::npos, "WGL_EXT_swap_control extension not supported.");

		static const PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = static_cast<PFNWGLSWAPINTERVALEXTPROC>(getProcedure("wglSwapIntervalEXT"));

		result = wglSwapIntervalEXT(interval);

		WIN32_ASSERT(result == TRUE);
	} */

	ref<Trident::Instance> GL::createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);
		BOOL result;

		ref<GL::Instance> instance = createRef<GL::Instance>(adapter);
		instance->deviceContext = windowData->hDC;

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

		const sint32 pixelFormat = ChoosePixelFormat(instance->deviceContext, &pixelFormatDescriptor);

		WIN32_ASSERT(pixelFormat != 0);

		result = SetPixelFormat(instance->deviceContext, pixelFormat, &pixelFormatDescriptor);

		WIN32_ASSERT(result == TRUE);

		const PFNWGLCREATECONTEXTPROC wglCreateContext = static_cast<PFNWGLCREATECONTEXTPROC>(getWglProcedure("wglCreateContext"));
		const HGLRC dummyHGLRC = wglCreateContext(instance->deviceContext);

		WIN32_ASSERT(instance->glRenderingContext != nullptr);

		instance->wglMakeCurrent = static_cast<PFNWGLMAKECURRENTPROC>(getWglProcedure("wglMakeCurrent"));

		result = instance->wglMakeCurrent(instance->deviceContext, dummyHGLRC);

		WIN32_ASSERT(result == TRUE);

		const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = static_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(getProcedure("wglCreateContextAttribsARB"));

		result = instance->wglMakeCurrent(instance->deviceContext, nullptr);

		WIN32_ASSERT(result == TRUE);

		instance->wglDeleteContext = static_cast<PFNWGLDELETECONTEXTPROC>(getWglProcedure("wglDeleteContext"));

		result = instance->wglDeleteContext(dummyHGLRC);

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

		instance->glRenderingContext = wglCreateContextAttribsARB(instance->deviceContext, nullptr, attributes);

		WIN32_ASSERT(instance->glRenderingContext != nullptr);

		result = wglMakeCurrent(instance->deviceContext, instance->glRenderingContext);

		WIN32_ASSERT(result == TRUE);

		// instance->loadProcedures();

		return instance;
	}

	void GL::destroyInstance(const Trident::Instance* instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Instance* _instance = static_cast<const GL::Instance*>(instance);
		BOOL result;

		result = _instance->wglMakeCurrent(_instance->deviceContext, nullptr);

		WIN32_ASSERT(result == TRUE);

		result = _instance->wglDeleteContext(_instance->glRenderingContext);

		WIN32_ASSERT(result == TRUE);
	}
}