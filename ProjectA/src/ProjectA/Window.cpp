#include "pch.hpp"

#include "ProjectA/Window.hpp"

#ifdef PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace ProjectA {
	Scope<Window> Window::create(const WindowData& data) {
		#ifdef PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(data);
		#else
			ENGINE_ASSERT(false, "Platform not supported.");

			return nullptr;
		#endif
	}
}