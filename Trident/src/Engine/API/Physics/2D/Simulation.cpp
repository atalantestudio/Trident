// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	PhysicsSimulation2D createSimulation(const PhysicsSimulation2DDescriptor& descriptor) {
		PhysicsSimulation2D simulation;
		simulation.deltaTime = static_cast<float32>(descriptor.deltaTime);
		simulation.inverseDeltaTime = static_cast<float32>(1.0 / descriptor.deltaTime);
		simulation.gravity = descriptor.gravity;
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

		// TODO
		// Create index arrays for each body type (static, kinematic, dynamic)
		// Broad phase: collect every static-dynamic and dynamic-dynamic pair
		// Narrow phase: process pairs
		// Update manifolds: update intersecting pairs

		return simulation;
	}

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBodyDescriptor2D& descriptor) {
		DEBUG_ASSERT(!std::isinf(descriptor.position.x) && !std::isnan(descriptor.position.x) && !std::isinf(descriptor.position.y) && !std::isnan(descriptor.position.y));
		DEBUG_ASSERT(!std::isinf(descriptor.rotation) && !std::isnan(descriptor.rotation));
		DEBUG_ASSERT(!std::isinf(descriptor.mass) && !std::isnan(descriptor.mass) && descriptor.mass > 0.0_kg);

		ENGINE_ASSERT(simulation.usedRigidBodyCount + 1 < simulation.rigidBodies.count(), "Rigid body count limit already reached.");

		if (descriptor.type == RigidBodyType::DYNAMIC) {
			if (descriptor.mass == max<float32>()) {
				ENGINE_WARNING("Dynamic bodies should have non-infinite mass.");
			}

			if (descriptor.inertia == max<float32>()) {
				ENGINE_WARNING("Dynamic bodies should have non-infinite inertia.");
			}
		}

		RigidBody2D& rigidBody = simulation.rigidBodies[simulation.usedRigidBodyCount];
		rigidBody.type = descriptor.type;
		rigidBody.position = descriptor.position;
		rigidBody.rotation = descriptor.rotation;
		rigidBody.centroid = {0.0_m, 0.0_m};
		rigidBody.flags = descriptor.flags;
		rigidBody.force = {0.0f, 0.0f};
		rigidBody.torque = 0.0_Nm;
		rigidBody.inverseMass = 0.0_kg;

		if (descriptor.mass != max<float32>()) {
			rigidBody.inverseMass = 1.0f / descriptor.mass;
		}

		rigidBody.inverseInertia = 0.0f;

		if (descriptor.inertia != max<float32>()) {
			rigidBody.inverseInertia = 1.0f / descriptor.inertia;
		}

		rigidBody.state.linearVelocity = {0.0_m_s, 0.0_m_s};
		rigidBody.state.angularVelocity = 0.0f;

		const uint32 id = simulation.usedRigidBodyCount;

		simulation.usedRigidBodyCount += 1;

		return id;
	}

	ShapeID2D createShape(PhysicsSimulation2D& simulation, const ShapeDescriptor2D& descriptor, void* typeDescriptor) {
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
				// TODO(physics/2d/shape/convex-hull)
				ENGINE_ERROR_UNIMPLEMENTED();

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];
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
}