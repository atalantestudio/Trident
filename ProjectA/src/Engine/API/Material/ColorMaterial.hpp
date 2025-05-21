#pragma once

#include "Engine/API/Material/Material.hpp"

namespace ProjectA {
	struct ColorMaterialDescriptor {
		vec4<float32> fillColor;
		vec4<float32> strokeColor;
	};

	class ColorMaterial : public Material {
		public:
			ColorMaterial(const ColorMaterialDescriptor& descriptor) :
				fillColor(descriptor.fillColor),
				strokeColor(descriptor.strokeColor)
			{}

		public:
			const vec4<float32> fillColor;
			const vec4<float32> strokeColor;
	};
}