#pragma once

namespace ProjectA {
	enum class GraphicsAPI : uint8 {
		// CPU backend (headless graphics). Not supported for now.
		NONE = 0,

		#ifdef ENABLE_OPENGL
			// OpenGL 4.6.
			OPENGL = 1,
		#endif

		#ifdef ENABLE_VULKAN
			// Vulkan 1.
			VULKAN = 2,
		#endif
	};
}