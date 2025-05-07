#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	const Geometry2D& Scene2D::getGeometry(uint32 index) const {
		ENGINE_ASSERT(index < geometries.size(), "Index overflow");

		return geometries[index];
	}

	const ref<Material>& Scene2D::getMaterial(uint32 index) const {
		ENGINE_ASSERT(index < materials.size(), "Index overflow.");

		return materials[index];
	}

	uint32 Scene2D::registerGeometry(const Geometry2D& geometry) {
		const uint32 geometryIndex = static_cast<uint32>(geometries.size());

		geometries.emplace_back(geometry);

		return geometryIndex;
	}

	uint32 Scene2D::registerMaterial(const ref<Material>& material) {
		const uint32 materialIndex = static_cast<uint32>(materials.size());

		materials.emplace_back(material);

		return materialIndex;
	}

	uint32 Scene2D::registerObject(const Object2D& object) {
		const uint32 objectIndex = static_cast<uint32>(objects.size());

		objects.emplace_back(object);

		return objectIndex;
	}

	std::vector<Object2D>::const_iterator Scene2D::begin() const {
		return objects.begin();
	}

	std::vector<Object2D>::const_iterator Scene2D::end() const {
		return objects.end();
	}
}