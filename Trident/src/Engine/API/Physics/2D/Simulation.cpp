// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	PhysicsSimulation2D createSimulation(const PhysicsSimulation2DDescriptor& descriptor) {
		PhysicsSimulation2D simulation;
		simulation.deltaTime = descriptor.deltaTime;
		simulation.inverseDeltaTime = 1.0f / descriptor.deltaTime;
		simulation.gravity = descriptor.gravity;
		simulation.impulseIterationCount = descriptor.impulseIterationCount;
		simulation.rigidBodies = sequence<RigidBody2D>(descriptor.maxStaticRigidBodyCount + descriptor.maxKinematicAndDynamicRigidBodyCount);
		simulation.shapes = sequence<Shape2D>(descriptor.maxStaticShapeCount + descriptor.maxKinematicAndDynamicShapeCount);
		simulation.usedRigidBodyCount = 0;
		simulation.usedShapeCount = 0;
		simulation.broadPhase.staticShapeIds = sequence<uint32>(simulation.shapes.count());
		simulation.broadPhase.kinematicShapeIds = sequence<uint32>(simulation.shapes.count());
		simulation.broadPhase.dynamicShapeIds = sequence<uint32>(simulation.shapes.count());
		simulation.broadPhase.usedStaticShapeIdCount = 0;
		simulation.broadPhase.usedKinematicShapeIdCount = 0;
		simulation.broadPhase.usedDynamicShapeIdCount = 0;
		simulation.collisionPairs = {};
		simulation.contactPairs = {};

		return simulation;
	}

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBodyDescriptor2D& descriptor) {
		DEBUG_ASSERT(!std::isinf(descriptor.position.x) && !std::isnan(descriptor.position.x) && !std::isinf(descriptor.position.y) && !std::isnan(descriptor.position.y));
		DEBUG_ASSERT(!std::isinf(descriptor.rotation) && !std::isnan(descriptor.rotation));

		ENGINE_ASSERT(simulation.usedRigidBodyCount + 1 < simulation.rigidBodies.count(), "Rigid body count limit already reached.");

		RigidBody2D& rigidBody = simulation.rigidBodies[simulation.usedRigidBodyCount];
		rigidBody.type = descriptor.type;
		rigidBody.flags = descriptor.flags;
		rigidBody.position = descriptor.position;
		rigidBody.centroid = {0.0_m, 0.0_m};
		rigidBody.rotation = descriptor.rotation;
		rigidBody.inverseMass = 0.0_kg;
		rigidBody.inverseInertia = 0.0f;
		rigidBody.linearVelocity = descriptor.linearVelocity;
		rigidBody.angularVelocity = descriptor.angularVelocity;
		rigidBody.force = {0.0f, 0.0f};
		rigidBody.torque = 0.0_Nm;

		const uint32 id = simulation.usedRigidBodyCount;

		simulation.usedRigidBodyCount += 1;

		return id;
	}

	ShapeID2D createShape(PhysicsSimulation2D& simulation, const ShapeDescriptor2D& descriptor, const void* typeDescriptor) {
		DEBUG_ASSERT(!std::isinf(descriptor.density) && !std::isnan(descriptor.density) && descriptor.density >= 0.0f);
		DEBUG_ASSERT(!std::isinf(descriptor.material.friction) && !std::isnan(descriptor.material.friction) && descriptor.material.friction >= 0.0f);
		DEBUG_ASSERT(!std::isinf(descriptor.material.restitution) && !std::isnan(descriptor.material.restitution) && descriptor.material.restitution >= 0.0f);

		ENGINE_ASSERT(simulation.usedShapeCount + 1 < simulation.shapes.count(), "Shape count limit already reached.");
		ENGINE_ASSERT(descriptor.rigidBodyId < simulation.usedRigidBodyCount, "Invalid rigid body ID.");

		Shape2D& shape = simulation.shapes[simulation.usedShapeCount];
		shape.type = descriptor.type;
		shape.rigidBodyId = descriptor.rigidBodyId;
		shape.material = descriptor.material;
		shape.density = descriptor.density;

		RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

		switch (shape.type) {
			case ShapeType2D::CIRCLE: {
				shape.circle = *static_cast<const CircleShape2D*>(typeDescriptor);

				break;
			}
			case ShapeType2D::CAPSULE: {
				// TODO(physics/2d/shape/capsule)
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case ShapeType2D::CONVEX_HULL: {
				shape.hull = static_cast<const HullHeader2D*>(typeDescriptor);

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		updateMassAndInertia(simulation, shape.rigidBodyId);

		const uint32 id = simulation.usedShapeCount;

		switch (rigidBody.type) {
			case RigidBodyType::STATIC: {
				simulation.broadPhase.staticShapeIds[simulation.broadPhase.usedStaticShapeIdCount] = id;
				simulation.broadPhase.usedStaticShapeIdCount += 1;

				break;
			}
			case RigidBodyType::KINEMATIC: {
				simulation.broadPhase.kinematicShapeIds[simulation.broadPhase.usedKinematicShapeIdCount] = id;
				simulation.broadPhase.usedKinematicShapeIdCount += 1;

				break;
			}
			case RigidBodyType::DYNAMIC: {
				simulation.broadPhase.dynamicShapeIds[simulation.broadPhase.usedDynamicShapeIdCount] = id;
				simulation.broadPhase.usedDynamicShapeIdCount += 1;

				break;
			}
		}

		simulation.usedShapeCount += 1;

		return id;
	}

	BoxHull2D createBoxHull(vec2<float32> halfSize) {
		static uint32 userIndex = 1;

		const float32 hx = halfSize.x;
		const float32 hy = halfSize.y;

		const float32 x = hx * 2.0f;
		const float32 y = hy * 2.0f;

		BoxHull2D boxHull;
		boxHull.header.centroid = {0.0f, 0.0f};
		boxHull.header.area = x * y;
		boxHull.header.radius = length(halfSize);
		boxHull.header.inertia = (x * x + y * y) / 12.0f;
		boxHull.header.vertexCount = 4;
		boxHull.header.vertexOffset = offsetof(BoxHull2D, vertices);
		boxHull.header.userIndex = userIndex;
		boxHull.vertices[0] = {-hx,  hy};
		boxHull.vertices[1] = { hx,  hy};
		boxHull.vertices[2] = { hx, -hy};
		boxHull.vertices[3] = {-hx, -hy};

		userIndex += 1;

		return boxHull;
	}

	void updateMassAndInertia(PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId) {
		RigidBody2D& rigidBody = simulation.rigidBodies[rigidBodyId];
		rigidBody.inverseMass = 0.0f;
		rigidBody.inverseInertia = 0.0f;

		if (rigidBody.type != RigidBodyType::DYNAMIC) {
			return;
		}

		const Shape2D& shape = simulation.shapes[simulation.usedShapeCount];

		float32 m;
		float32 I;

		switch (shape.type) {
			case ShapeType2D::CIRCLE: {
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case ShapeType2D::CAPSULE: {
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			case ShapeType2D::CONVEX_HULL: {
				m = shape.hull->area * shape.density;
				I = shape.hull->inertia * shape.density;

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		rigidBody.inverseMass = 1.0f / m;
		rigidBody.inverseInertia = 1.0f / I;
	}
}