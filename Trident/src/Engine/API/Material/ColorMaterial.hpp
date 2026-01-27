#pragma once

#include "Engine/API/Material/Material.hpp"

namespace Trident {
	struct ColorMaterial : public Material {
		vec4<float32> color;
	};
}