// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	PhysicsSimulation2D createSimulation(const PhysicsSimulationDescriptor2D& descriptor) {
		PhysicsSimulation2D simulation;
		simulation.deltaTime = static_cast<float32>(descriptor.deltaTime);
		simulation.inverseDeltaTime = static_cast<float32>(1.0 / descriptor.deltaTime);
		simulation.gravity = descriptor.gravity;
		simulation.allowedPenetration = descriptor.allowedPenetration;
		simulation.impulseIterationCount = descriptor.impulseIterationCount;
		simulation.tick = 0;

		uint32 maxStaticRigidBodyCount = descriptor.maxStaticRigidBodyCount;
		uint32 maxKinematicAndDynamicRigidBodyCount = descriptor.maxKinematicAndDynamicRigidBodyCount;
		uint32 maxStaticShapeCount = descriptor.maxStaticShapeCount;
		uint32 maxKinematicAndDynamicShapeCount = descriptor.maxKinematicAndDynamicShapeCount;

		simulation.rigidBodies = sequence<RigidBody2D>(maxStaticRigidBodyCount + maxKinematicAndDynamicRigidBodyCount);
		simulation.shapes = sequence<Shape2D>(maxStaticShapeCount + maxKinematicAndDynamicShapeCount);
		simulation.usedRigidBodyCount = 0;
		simulation.usedShapeCount = 0;
		simulation.broadPhase.staticShapeIds = sequence<uint32>(maxStaticShapeCount);
		simulation.broadPhase.kinematicShapeIds = sequence<uint32>(maxKinematicAndDynamicShapeCount);
		simulation.broadPhase.dynamicShapeIds = sequence<uint32>(maxKinematicAndDynamicShapeCount);
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

		ENGINE_ASSERT(simulation.usedRigidBodyCount < simulation.rigidBodies.count(), "Rigid body count limit already reached.");

		RigidBody2D& rigidBody = simulation.rigidBodies[simulation.usedRigidBodyCount];
		rigidBody.type = descriptor.type;
		rigidBody.flags = descriptor.flags;
		rigidBody.centroid = {0.0_m, 0.0_m};
		rigidBody.transform.position = descriptor.position;
		rigidBody.rotation = descriptor.rotation;
		rigidBody.transform.rotation = descriptor.rotation;
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

		ENGINE_ASSERT(simulation.usedShapeCount < simulation.shapes.count(), "Shape count limit already reached.");
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
			case ShapeType2D::CONVEX_HULL: {
				shape.hull = static_cast<const HullHeader2D*>(typeDescriptor);

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		RigidBody2D& rigidBody = simulation.rigidBodies[descriptor.rigidBodyId];

		if (rigidBody.type == RigidBodyType::DYNAMIC) {
			updateMassProperties(shape, rigidBody);
		}

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

	void tick(PhysicsSimulation2D& simulation) {
		simulation.collisionPairs.clear();

		broadPhase(simulation);
		narrowPhase(simulation);
		integrateVelocities(simulation);
		updateContactManifolds(simulation);

		// TODO(physics/2d/joint): Restore joints.
		// updateJoints(simulation);

		applyImpulses(simulation);
		integratePositions(simulation);

		simulation.tick += 1;
	}

	void applyImpulse(RigidBody2D& rigidBody, vec2<float32> j, vec2<float32> r) {
		rigidBody.linearVelocity += j * rigidBody.inverseMass;
		rigidBody.angularVelocity -= parallelogramArea(r, j) * rigidBody.inverseInertia;
	}
}