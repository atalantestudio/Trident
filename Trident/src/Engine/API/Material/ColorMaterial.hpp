// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/PhysicsMaterial.hpp"

namespace Trident {
	struct ColorMaterial : public PhysicsMaterial {
		vec4<float32> color;
	};
}