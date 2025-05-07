#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	uint32 Scene2D::registerGeometry(const Geometry2D& geometry) {
		const uint32 geometryIndex = geometries.size();

		geometries.emplace_back(geometry);

		return geometryIndex;
	}

	uint32 Scene2D::registerMaterial(const ref<Material>& material) {
		const uint32 materialIndex = materials.size();

		materials.emplace_back(material);

		return materialIndex;
	}

	uint32 Scene2D::registerObject(const Object2D& object) {
		const uint32 objectIndex = objects.size();

		objects.emplace_back(object);

		return objectIndex;
	}
}