#pragma once

namespace ProjectA {
	enum class GraphicsAPI : uint8 {
		// CPU backend (headless graphics). Not supported for now.
		NONE = 0,

		#ifdef ENABLE_OPENGL
			// OpenGL graphics API. Fast, very good support.
			OPENGL = 1,
		#endif

		#ifdef ENABLE_VULKAN
			// Vulkan graphics API. Very fast, not very good support.
			VULKAN = 2,
		#endif
	};
}