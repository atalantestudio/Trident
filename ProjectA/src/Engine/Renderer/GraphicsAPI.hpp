#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class GraphicsAPI : uint32 {
		// CPU backend. Not supported for now.
		NONE = 0,
		// OpenGL 4.5.
		OPENGL = 1,
		// Vulkan 1.
		VULKAN = 2,
	};
}