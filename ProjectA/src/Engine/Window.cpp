#include "pch.hpp"

#include "Engine/Window.hpp"

#ifdef PLATFORM_WINDOWS
	#include "Backend/Windows/WindowsWindow.hpp"
#endif

namespace ProjectA {
	scope<Window> Window::create(const WindowData& data) {
		#ifdef PLATFORM_WINDOWS
			return createScope<WindowsWindow>(data);
		#else
			ENGINE_ASSERT(false, "Platform not supported.");

			return nullptr;
		#endif
	}
}