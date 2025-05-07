#include "pch.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"

namespace ProjectA {
	ColorMaterial::ColorMaterial(const ColorMaterialDescriptor& descriptor) :
		fillColor(descriptor.fillColor),
		strokeColor(descriptor.strokeColor)
	{}
}