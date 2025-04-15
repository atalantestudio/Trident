#pragma once

namespace ProjectA {
	enum class GraphicsAPI : uint8 {
		// CPU backend. Not supported for now.
		NONE = 0,
		// OpenGL 4.5.
		OPENGL = 1,
		// Vulkan 1.
		VULKAN = 2,
	};
}