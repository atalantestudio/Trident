#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Instance.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

namespace ProjectA {
	void* OpenGLInstance::getWglProcedure(view<char8> procedureName) {
		static const PlatformLibrary opengl32Library = Platform::openLibrary("opengl32.dll");

		return opengl32Library.getProcedure(procedureName);
	}

	OpenGLInstance::OpenGLInstance(const Window& window) {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);

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
		pfd.iLayerType = 0;
		pfd.bReserved = 0;
		pfd.dwLayerMask = 0;
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;

		const sint32 format = ChoosePixelFormat(hDC, &pfd);

		WIN32_ASSERT(format != 0);

		WIN32_ASSERT(SetPixelFormat(hDC, format, &pfd) == TRUE);

		const PFNWGLCREATECONTEXTPROC wglCreateContext = static_cast<PFNWGLCREATECONTEXTPROC>(getWglProcedure("wglCreateContext"));

		const HGLRC dummyHGLRC = wglCreateContext(hDC);

		WIN32_ASSERT(hGLRC != nullptr);

		wglMakeCurrent = static_cast<PFNWGLMAKECURRENTPROC>(getWglProcedure("wglMakeCurrent"));

		WIN32_ASSERT(wglMakeCurrent(hDC, dummyHGLRC) == TRUE);

		const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = static_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(getProcedure("wglCreateContextAttribsARB"));

		WIN32_ASSERT(wglMakeCurrent(hDC, nullptr) == TRUE);

		wglDeleteContext = static_cast<PFNWGLDELETECONTEXTPROC>(getWglProcedure("wglDeleteContext"));

		WIN32_ASSERT(wglDeleteContext(dummyHGLRC) == TRUE);

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

	OpenGLInstance::~OpenGLInstance() {
		WIN32_ASSERT(wglMakeCurrent(hDC, nullptr) == TRUE);
		WIN32_ASSERT(wglDeleteContext(hGLRC) == TRUE);
	}

	void* OpenGLInstance::getProcedure(view<char8> procedureName) const {
		static const PFNWGLGETPROCADDRESSPROC wglGetProcAddress = static_cast<PFNWGLGETPROCADDRESSPROC>(getWglProcedure("wglGetProcAddress"));

		const PROC procedure = wglGetProcAddress(&procedureName[0]);

		if (procedure == nullptr || procedure == (void*) -1 || procedure == (void*) 1 || procedure == (void*) 2 || procedure == (void*) 3) {
			return getWglProcedure(procedureName);
		}

		return procedure;
	}

	void OpenGLInstance::makeCurrent() const {
		WIN32_ASSERT(wglMakeCurrent(hDC, hGLRC) == TRUE);
	}

	void OpenGLInstance::setSwapInterval(sint32 interval) const {
		static const PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = static_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(getProcedure("wglGetExtensionsStringEXT"));
		static const std::string extensionsString = wglGetExtensionsStringEXT();

		ENGINE_ASSERT(extensionsString.find("WGL_EXT_swap_control") != std::string::npos, "WGL_EXT_swap_control extension not supported.");

		static const PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = static_cast<PFNWGLSWAPINTERVALEXTPROC>(getProcedure("wglSwapIntervalEXT"));

		WIN32_ASSERT(wglSwapIntervalEXT(interval) == TRUE);
	}

	void OpenGLInstance::swapBuffers() const {
		WIN32_ASSERT(SwapBuffers(hDC) == TRUE);
	}
}