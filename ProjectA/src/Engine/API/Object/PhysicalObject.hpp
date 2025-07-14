#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/ECS/Entity.hpp"

namespace ProjectA {
	class Scene2D;

	typedef mat3x3<float32> Transform3D;

	struct PhysicalObject2D {
		Transform3D transform = mat3x3<float32>::identity();
	};

	struct PhysicalObjectCopy2D {
		Entity entity;

		Transform3D transform;

		vec2<float32> position;
		vec2<float32> linearVelocity;
		vec2<float32> force;

		float32 rotation;
		float32 angularVelocity;
		float32 torque;

		vec2<float32> scale;

		ref<Geometry2D> geometry;
	};

	extern PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity);
	extern PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity, float32 t);
}