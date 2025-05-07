#include "pch.hpp"

#include "Engine/API/Object/Object2D.hpp"

namespace ProjectA {
	Object2D::Object2D(const Object2DDescriptor& descriptor) :
		geometryIndex(descriptor.geometryIndex),
		materialIndex(descriptor.materialIndex)
	{}
}