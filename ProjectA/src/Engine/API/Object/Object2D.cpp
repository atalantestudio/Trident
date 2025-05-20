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

	void Object2D::updateTransform() {
		const ref<Geometry2D>& geometry = getGeometry();

		transform =
			mat3x3<float32>::translation(-geometry->centerOfMass) *
			mat3x3<float32>::scaling(scale) *
			mat3x3<float32>::rotation(rotation) *
			mat3x3<float32>::translation(position);
	}

	void Object2D::applyLinearForce(const vec2<float32>& linearForce) {
		linearAcceleration += linearForce;
	}

	void Object2D::applyAngularForce(float32 angularForce) {
		angularAcceleration += angularForce;
	}
}