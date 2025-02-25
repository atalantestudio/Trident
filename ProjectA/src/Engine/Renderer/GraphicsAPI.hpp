#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class GraphicsAPI : uint32 {
		// CPU backend. Not supported for now.
		NONE = 0,
		// TODO: Target OpenGL >= 4.
		OPENGL = 1,
		// Vulkan. Not fully supported.
		VULKAN = 2,
	};
}