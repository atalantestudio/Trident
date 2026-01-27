// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"

namespace Trident {
	struct ColorFillStrokeMaterial : public Material {
		vec4<float32> fillColor;
		vec4<float32> strokeColor;
	};
}