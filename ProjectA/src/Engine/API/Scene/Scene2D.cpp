#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	uint32 Scene2D::getObjectCount() const {
		return static_cast<uint32>(objects.size());
	}

	const Geometry2D& Scene2D::getGeometry(uint32 index) const {
		DEBUG_ASSERT(index < geometries.size());

		return geometries[index];
	}

	const ref<Material>& Scene2D::getMaterial(uint32 index) const {
		DEBUG_ASSERT(index < materials.size());

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

	Object2D& Scene2D::registerObject(const Object2DDescriptor& descriptor) {
		objects.emplace_back(Object2D(descriptor));

		return objects.back();
	}

	void Scene2D::updateTransform(Object2D& object) const {
		const Geometry2D& geometry = geometries[object.geometryIndex];
		const vec2<float32> centerOfMass = {geometry.getCenterOfMass().x, geometry.getCenterOfMass().y};

		object.transform =
			mat3x3<float32>::translation(-centerOfMass) *
			mat3x3<float32>::scaling(object.scale) *
			mat3x3<float32>::rotation(object.rotation) *
			mat3x3<float32>::translation(object.position);
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