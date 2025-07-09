#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity) {
		const ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene.getBody(entity));
		const ref<Geometry2D>& geometry = scene.getGeometry(entity);
		const PhysicalObject2D& physicalObject = scene.getPhysicalObject(entity);

		PhysicalObjectCopy2D copy{};
		copy.transform = physicalObject.transform;
		copy.position = rigidBody->position;
		copy.linearVelocity = rigidBody->linearVelocity;
		copy.linearAcceleration = rigidBody->linearAcceleration;
		copy.rotation = rigidBody->rotation;
		copy.angularVelocity = rigidBody->angularVelocity;
		copy.angularAcceleration = rigidBody->angularAcceleration;
		copy.scale = rigidBody->scale;
		copy.geometry = geometry;

		return copy;
	}

	PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity, float32 t) {
		PhysicalObjectCopy2D copy = copyPhysicalObject(scene, entity);

		copy.position += copy.linearVelocity * t + copy.linearAcceleration * t * t * 0.5f;
		copy.rotation += copy.angularVelocity * t + copy.angularAcceleration * t * t * 0.5f;

		copy.linearVelocity += copy.linearAcceleration * t;
		copy.angularVelocity += copy.angularAcceleration * t;

		copy.transform =
			mat3x3<float32>::translation(-copy.geometry->centerOfMass) *
			mat3x3<float32>::scaling(copy.scale) *
			mat3x3<float32>::rotation(copy.rotation) *
			mat3x3<float32>::translation(copy.position);

		return copy;
	}
}