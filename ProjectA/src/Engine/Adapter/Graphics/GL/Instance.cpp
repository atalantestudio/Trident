// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Window/Win32/base.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	void* GL::Instance::getWglProcedure(view<char8> procedureName) {
		static const PlatformLibrary opengl32Library = Platform::openLibrary("opengl32.dll");

		return opengl32Library.getProcedure(procedureName);
	}

	GL::Instance::Instance(const Window& window) {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);

		BOOL result;

		hDC = windowData->hDC;

		PIXELFORMATDESCRIPTOR pfd{};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cRedBits = 0;
		pfd.cRedShift = 0;
		pfd.cGreenBits = 0;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 0;
		pfd.cBlueShift = 0;
		pfd.cAlphaBits = 0;
		pfd.cAlphaShift = 0;
		pfd.cAccumBits = 0;
		pfd.cAccumRedBits = 0;
		pfd.cAccumGreenBits = 0;
		pfd.cAccumBlueBits = 0;
		pfd.cAccumAlphaBits = 0;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.cAuxBuffers = 0;
		pfd.iLayerType = PFD_MAIN_PLANE;
		pfd.bReserved = 0;
		pfd.dwLayerMask = 0;
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;

		const sint32 format = ChoosePixelFormat(hDC, &pfd);

		WIN32_ASSERT(format != 0);

		result = SetPixelFormat(hDC, format, &pfd);

		WIN32_ASSERT(result == TRUE);

		const PFNWGLCREATECONTEXTPROC wglCreateContext = static_cast<PFNWGLCREATECONTEXTPROC>(getWglProcedure("wglCreateContext"));

		const HGLRC dummyHGLRC = wglCreateContext(hDC);

		WIN32_ASSERT(hGLRC != nullptr);

		wglMakeCurrent = static_cast<PFNWGLMAKECURRENTPROC>(getWglProcedure("wglMakeCurrent"));

		result = wglMakeCurrent(hDC, dummyHGLRC);

		WIN32_ASSERT(result == TRUE);

		const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = static_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(getProcedure("wglCreateContextAttribsARB"));

		result = wglMakeCurrent(hDC, nullptr);

		WIN32_ASSERT(result == TRUE);

		wglDeleteContext = static_cast<PFNWGLDELETECONTEXTPROC>(getWglProcedure("wglDeleteContext"));

		result = wglDeleteContext(dummyHGLRC);

		WIN32_ASSERT(result == TRUE);

		#ifdef DEBUG_ENABLE_MESSENGER
			constexpr sint32 flags = WGL_CONTEXT_DEBUG_BIT_ARB;
		#else
			constexpr sint32 flags = 0;
		#endif

		constexpr sint32 attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_LAYER_PLANE_ARB, 0,
			WGL_CONTEXT_FLAGS_ARB, flags,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0,
		};

		hGLRC = wglCreateContextAttribsARB(hDC, nullptr, attribList);

		WIN32_ASSERT(hGLRC != nullptr);
	}

	GL::Instance::~Instance() {
		BOOL result;
		
		result = wglMakeCurrent(hDC, nullptr);

		WIN32_ASSERT(result == TRUE);

		result = wglDeleteContext(hGLRC);

		WIN32_ASSERT(result == TRUE);
	}

	void* GL::Instance::getProcedure(view<char8> procedureName) const {
		static const PFNWGLGETPROCADDRESSPROC wglGetProcAddress = static_cast<PFNWGLGETPROCADDRESSPROC>(getWglProcedure("wglGetProcAddress"));

		const PROC procedure = wglGetProcAddress(&procedureName[0]);

		if (procedure == nullptr || procedure == (void*) -1 || procedure == (void*) 1 || procedure == (void*) 2 || procedure == (void*) 3) {
			return getWglProcedure(procedureName);
		}

		return procedure;
	}

	void GL::Instance::makeCurrent() const {
		BOOL result;
		
		result = wglMakeCurrent(hDC, hGLRC);

		WIN32_ASSERT(result == TRUE);
	}

	void GL::Instance::setSwapInterval(sint32 interval) const {
		BOOL result;

		static const PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = static_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(getProcedure("wglGetExtensionsStringEXT"));
		static const std::string extensionsString = wglGetExtensionsStringEXT();

		ADAPTER_ASSERT(extensionsString.find("WGL_EXT_swap_control") != std::string::npos, "WGL_EXT_swap_control extension not supported.");

		static const PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = static_cast<PFNWGLSWAPINTERVALEXTPROC>(getProcedure("wglSwapIntervalEXT"));

		result = wglSwapIntervalEXT(interval);

		WIN32_ASSERT(result == TRUE);
	}

	void GL::Instance::swapBuffers() const {
		BOOL result;

		result = SwapBuffers(hDC);

		WIN32_ASSERT(result == TRUE);
	}
}