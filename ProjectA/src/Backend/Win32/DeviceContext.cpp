#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/DeviceContext.hpp"

namespace ProjectA {
	DeviceContext::DeviceContext(const Window& window) {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);
		Win32DeviceContextData* deviceContextData = new Win32DeviceContextData();

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

		deviceContextData->hDC = windowData->hDC;
		deviceContextData->hGLRC = windowData->wglCreateContextAttribsARB(windowData->hDC, nullptr, attribList);

		WIN32_ASSERT(deviceContextData->hGLRC != nullptr);

		WIN32_ASSERT(wglMakeCurrent(deviceContextData->hDC, deviceContextData->hGLRC) == TRUE);

		data = deviceContextData;
	}

	DeviceContext::~DeviceContext() {
		const Win32DeviceContextData* deviceContextData = static_cast<const Win32DeviceContextData*>(data);

		WIN32_ASSERT(wglMakeCurrent(deviceContextData->hDC, nullptr) == TRUE);

		WIN32_ASSERT(wglDeleteContext(deviceContextData->hGLRC) == TRUE);

		delete deviceContextData;

		data = nullptr;
	}

	void* DeviceContext::getProcedureAddress(std::string_view name) const {
		PROC procAddress = wglGetProcAddress(name.data());

		if (procAddress == nullptr) {
			const HMODULE hModule = LoadLibrary(L"opengl32.dll");

			WIN32_ASSERT(hModule != nullptr);

			procAddress = GetProcAddress(hModule, name.data());

			WIN32_ASSERT(procAddress != nullptr);
		}

		return procAddress;
	}

	void DeviceContext::swapBuffers() const {
		const Win32DeviceContextData* deviceContextData = static_cast<const Win32DeviceContextData*>(data);

		WIN32_ASSERT(SwapBuffers(deviceContextData->hDC) == TRUE);
	}
}