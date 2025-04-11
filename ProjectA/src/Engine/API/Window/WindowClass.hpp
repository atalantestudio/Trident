#pragma once

namespace ProjectA {
	struct WindowClassDescriptor {
		// The window class name.
		// NOTE: In Win32, the maximum class name length is 256.
		// Registering a window with a longer class name will fail.
		const char* name;
	};

	class WindowClass {
		public:
			WindowClass(const WindowClassDescriptor& descriptor);

			~WindowClass();

		public:
			const void* data;
	};
}