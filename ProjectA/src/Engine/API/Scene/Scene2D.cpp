#include "pch.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	uint32 Scene2D::getObjectCount() const {
		return static_cast<uint32>(objects.size());
	}

	const ref<Geometry2D>& Scene2D::getGeometry(uint32 index) const {
		DEBUG_ASSERT(index < geometries.size());

		return geometries[index];
	}

	const ref<Material>& Scene2D::getMaterial(uint32 index) const {
		DEBUG_ASSERT(index < materials.size());

		return materials[index];
	}

	Object2D& Scene2D::registerObject(const Object2DDescriptor& descriptor) {
		objects.emplace_back(Object2D(descriptor, this));

		return objects.back();
	}

	std::vector<Object2D>::iterator Scene2D::begin() {
		return objects.begin();
	}

	std::vector<Object2D>::const_iterator Scene2D::begin() const {
		return objects.begin();
	}

	std::vector<Object2D>::iterator Scene2D::end() {
		return objects.end();
	}

	std::vector<Object2D>::const_iterator Scene2D::end() const {
		return objects.end();
	}

	const Object2D& Scene2D::operator [](std::size_t index) const {
		DEBUG_ASSERT(index < objects.size());

		return objects[index];
	}

	Object2D& Scene2D::operator [](std::size_t index) {
		DEBUG_ASSERT(index < objects.size());

		return objects[index];
	}
}