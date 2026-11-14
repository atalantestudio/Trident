// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	static const Hull2D& getHull(const Scene2D* scene, Entity entity) {
		const view<Hull2D>& hulls = scene->getHulls();
		const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(entity);
		const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];

		ENGINE_ASSERT(geometry.hullIndex < hulls.count(), "The geometry doesn't have a hull.");

		return hulls[geometry.hullIndex];
	}

	PhysicsSimulation2D createSimulation(const PhysicsSimulationDescriptor2D& descriptor) {
		PhysicsSimulation2D simulation;
		simulation.deltaTime = static_cast<float32>(descriptor.deltaTime);
		simulation.inverseDeltaTime = static_cast<float32>(1.0 / descriptor.deltaTime);
		simulation.gravity = descriptor.gravity;
		simulation.allowedPenetration = descriptor.allowedPenetration;
		simulation.impulseIterationCount = descriptor.impulseIterationCount;
		simulation.tick = 0;

		const TagPool<StaticEntityTag>& staticPool = descriptor.scene->ecs.getTagPool<StaticEntityTag>();
		const TagPool<DynamicEntityTag>& dynamicPool = descriptor.scene->ecs.getTagPool<DynamicEntityTag>();

		simulation.rigidBodies = sequence<RigidBody2D>(staticPool.tagCount + dynamicPool.tagCount);
		simulation.shapes = sequence<Hull2D>(staticPool.tagCount + dynamicPool.tagCount);
		simulation.usedRigidBodyCount = 0;
		simulation.usedShapeCount = 0;
		simulation.broadPhase.staticShapeIds = sequence<uint32>(staticPool.tagCount);
		simulation.broadPhase.kinematicShapeIds = sequence<uint32>(0);
		simulation.broadPhase.dynamicShapeIds = sequence<uint32>(dynamicPool.tagCount);
		simulation.broadPhase.usedStaticShapeIdCount = 0;
		simulation.broadPhase.usedKinematicShapeIdCount = 0;
		simulation.broadPhase.usedDynamicShapeIdCount = 0;

		uint32 tagIndex = 0;

		while (tagIndex < staticPool.tagCount) {
			const Entity entity = staticPool.tagIndexToEntity[tagIndex];

			RigidBody2DComponent rigidBodyDescriptor = descriptor.scene->ecs.getComponent<RigidBody2DComponent>(entity);
			rigidBodyDescriptor.type = RigidBodyType::STATIC;

			const RigidBodyID2D rigidBodyId = createRigidBodyFromScene(simulation, rigidBodyDescriptor);

			Hull2D shapeDescriptor = getHull(descriptor.scene, rigidBodyId);
			shapeDescriptor.rigidBodyId = rigidBodyId;
			shapeDescriptor.material.friction = rigidBodyDescriptor.friction;

			createShapeFromScene(simulation, shapeDescriptor, nullptr);

			tagIndex += 1;
		}

		tagIndex = 0;

		while (tagIndex < dynamicPool.tagCount) {
			const Entity entity = dynamicPool.tagIndexToEntity[tagIndex];

			RigidBody2DComponent rigidBodyDescriptor = descriptor.scene->ecs.getComponent<RigidBody2DComponent>(entity);
			rigidBodyDescriptor.type = RigidBodyType::DYNAMIC;

			const RigidBodyID2D rigidBodyId = createRigidBodyFromScene(simulation, rigidBodyDescriptor);

			Hull2D shapeDescriptor = getHull(descriptor.scene, entity);
			shapeDescriptor.rigidBodyId = rigidBodyId;
			shapeDescriptor.material.friction = rigidBodyDescriptor.friction;

			createShapeFromScene(simulation, shapeDescriptor, nullptr);

			tagIndex += 1;
		}

		simulation.collisionPairs = {};
		simulation.contactPairs = {};

		return simulation;
	}

	RigidBodyID2D createRigidBodyFromScene(PhysicsSimulation2D& simulation, const RigidBody2DComponent& descriptor) {
		DEBUG_ASSERT(!std::isinf(descriptor.position.x) && !std::isnan(descriptor.position.x) && !std::isinf(descriptor.position.y) && !std::isnan(descriptor.position.y));
		DEBUG_ASSERT(!std::isinf(descriptor.rotation) && !std::isnan(descriptor.rotation));

		ENGINE_ASSERT(simulation.usedRigidBodyCount < simulation.rigidBodies.count(), "Rigid body count limit already reached.");

		RigidBody2D& rigidBody = simulation.rigidBodies[simulation.usedRigidBodyCount];
		rigidBody.type = descriptor.type;

		// TODO(wip)
		// rigidBody.flags = {};

		rigidBody.position = descriptor.position;
		rigidBody.rotation = descriptor.rotation;
		rigidBody.scale = descriptor.scale;
		rigidBody.centroid = {0.0_m, 0.0_m};
		rigidBody.inverseMass = 0.0_kg;
		rigidBody.inverseInertia = 0.0f;
		rigidBody.linearVelocity = descriptor.linearVelocity;
		rigidBody.angularVelocity = descriptor.angularVelocity;
		rigidBody.force = {0.0f, 0.0f};
		rigidBody.torque = 0.0_Nm;

		// TODO(wip)
		rigidBody.inverseMass = descriptor.inverseMass;
		rigidBody.inverseInertia = descriptor.inverseInertia;

		const uint32 id = simulation.usedRigidBodyCount;

		simulation.usedRigidBodyCount += 1;

		return id;
	}

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBodyDescriptor2D& descriptor) {
		// TODO(wip)
		ENGINE_ERROR_UNIMPLEMENTED();

		return 0;
	}

	ShapeID2D createShapeFromScene(PhysicsSimulation2D& simulation, const Hull2D& descriptor, const void* typeDescriptor) {
		// TODO(wip)
		// DEBUG_ASSERT(!std::isinf(descriptor.density) && !std::isnan(descriptor.density) && descriptor.density >= 0.0f);

		DEBUG_ASSERT(!std::isinf(descriptor.material.friction) && !std::isnan(descriptor.material.friction) && descriptor.material.friction >= 0.0f);

		ENGINE_ASSERT(simulation.usedShapeCount < simulation.shapes.count(), "Shape count limit already reached.");
		ENGINE_ASSERT(descriptor.rigidBodyId < simulation.usedRigidBodyCount, "Invalid rigid body ID.");

		simulation.shapes[simulation.usedShapeCount] = descriptor;

		RigidBody2D& rigidBody = simulation.rigidBodies[descriptor.rigidBodyId];
		rigidBody.centroid = descriptor.centroid;

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

	ShapeID2D createShape(PhysicsSimulation2D& simulation, const ShapeDescriptor2D& descriptor, const void* typeDescriptor) {
		// TODO(wip)
		ENGINE_ERROR_UNIMPLEMENTED();

		return 0;
	}

	BoxHull2D createBoxHull(vec2<float32> halfSize) {
		// TODO(wip)
		ENGINE_ERROR_UNIMPLEMENTED();

		return {};
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