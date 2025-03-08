#pragma once

#include "pch.hpp"

namespace ProjectA {
	// Simple quad for 2D GUIs.
	struct GUIComponent {
		mat3x3<float32> transform;
		uint32 colorIndex = 0;
		uint32 textureIndex = 0;
	};
}