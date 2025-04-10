#include "pch.hpp"

#ifdef PLATFORM_WINDOWS
	#include "Backend/Win32/Win32Window.hpp"
#endif

#include "Engine/Window.hpp"

namespace ProjectA {
	scope<Window> Window::create(const WindowDescriptor& descriptor) {
		#ifdef PLATFORM_WINDOWS
			return createScope<Win32Window>(descriptor);
		#else
			ENGINE_ASSERT(false, "Unsupported backend.");

			return nullptr;
		#endif
	}
}