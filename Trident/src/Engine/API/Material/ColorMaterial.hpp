// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"

namespace Trident {
	struct ColorMaterial : public Material {
		vec4<float32> color;
	};
}