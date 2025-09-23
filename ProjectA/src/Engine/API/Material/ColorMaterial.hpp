#pragma once

#include "Engine/API/Material/Material.hpp"

namespace ProjectA {
	struct ColorMaterial : public Material {
		vec4<float32> color;
	};
}