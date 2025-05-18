#include "pch.hpp"

#include "Engine/API/Object/Object2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	Object2D::Object2D(const Object2DDescriptor& descriptor, const Scene2D* scene) :
		geometryIndex(descriptor.geometryIndex),
		materialIndex(descriptor.materialIndex),
		scene(scene)
	{}

	const ref<Geometry2D>& Object2D::getGeometry() const {
		return scene->getGeometry(geometryIndex);
	}

	const ref<Material>& Object2D::getMaterial() const {
		return scene->getMaterial(materialIndex);
	}
}