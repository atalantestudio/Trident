#include "pch.hpp"

#include "ProjectA/Input.hpp"

#ifdef WINDOWS
	#include "Platform/Windows/WindowsInput.hpp"
#endif

namespace ProjectA {
	Scope<Input> Input::instance = Input::create();

	Scope<Input> Input::create() {
		#ifdef WINDOWS
			return CreateScope<WindowsInput>();
		#else
			ENGINE_ASSERT(false, "Platform not supported.");

			return nullptr;
		#endif
	}
}