#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/PhysicsEngine.hpp"

namespace ProjectA {
	void PhysicsEngine::registerScene(Scene2D& scene) {
		this->scene = &scene;
	}

	void PhysicsEngine::tick(
		float32 deltaTime,
		float32 inverseDeltaTime
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const float32 halfDeltaTime = deltaTime * 0.5f;

		const ComponentPool<DynamicEntityComponent>& dynamicPool = scene->getDynamicEntityPool();

		// 1. Broad phase: Detect potentially overlapping pairs.
		// TODO: If less than 2 objects in the same layer AND in different physical groups, no chance of overlap.
		const std::vector<EntityPair> entityPairs = BroadPhase();

		// 2. Narrow phase: Calculate the contact manifold for each pair.
		std::vector<Arbiter2D> arbiters = NarrowPhase(
			entityPairs
			#ifdef BUILD_MODE_DEBUG
				, visualizationData
			#endif
		);

		// 3. Integrate forces.
		// TODO: Do we need to integrate forces on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < dynamicPool.componentCount; componentIndex += 1) {
			const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			// TODO: Exclude dynamic objects with an inverse mass of 0 from the loop.
			if (rigidBody->inverseMass == 0.0f) {
				continue;
			}

			// NOTE: https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
			//       This seems like Velocity Verlet to me?
			// rigidBody->linearVelocity += (rigidBody->force * rigidBody->inverseMass + G) * halfDeltaTime;
			// rigidBody->angularVelocity += rigidBody->torque * rigidBody->inverseInertia * halfDeltaTime;
			rigidBody->linearVelocity += (rigidBody->force * rigidBody->inverseMass + G) * deltaTime;
			rigidBody->angularVelocity += rigidBody->torque * rigidBody->inverseInertia * deltaTime;
		}

		// 4. Pre-compute step:
		//    - Calculate combined values in each pair (static/dynamic friction, restitution, etc.)
		//    - For each contact, calculate mass normal, mass tangent and bias value
		for (Arbiter2D& arbiter : arbiters) {
			arbiter.precompute(inverseDeltaTime);
		}

		// 5. Apply normal and friction impulses to overlapping pairs in a sequential manner.
		for (uint32 iteration = 0; iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT; iteration += 1) {
			for (Arbiter2D& arbiter : arbiters) {
				arbiter.applyImpulse();
			}
		}

		// 6. Integrate positions/rotations and velocities and reset forces.
		// TODO: Do we need to integrate positions/rotations on dynamic objects with an inverse mass of 0?
		// TODO: Do we need to integrate positions/rotations on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < dynamicPool.componentCount; componentIndex += 1) {
			const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			rigidBody->position += rigidBody->linearVelocity * deltaTime;
			rigidBody->rotation += rigidBody->angularVelocity * deltaTime;

			/* if (rigidBody->inverseMass > 0.0f) {
				rigidBody->linearVelocity += (rigidBody->force * rigidBody->inverseMass + G) * halfDeltaTime;
				rigidBody->angularVelocity += rigidBody->torque * rigidBody->inverseInertia * halfDeltaTime;
			} */

			rigidBody->force = {0.0f, 0.0f};
			rigidBody->torque = 0.0f;

			scene->updateTransform(entity);
		}
	}

	std::vector<EntityPair> PhysicsEngine::BroadPhase() const {
		const ComponentPool<DynamicEntityComponent>& dynamicPool = scene->getDynamicEntityPool();
		const ComponentPool<StaticEntityComponent>& staticPool = scene->getStaticEntityPool();

		std::vector<EntityPair> entityPairs;
		EntityPair entityPair;

		// TODO: If less than 2 objects in the same layer AND in different physical groups, no chance of overlap.
		for (uint32 indexA = 0; indexA < dynamicPool.componentCount; indexA += 1) {
			entityPair.A = dynamicPool.componentIndexToEntity[indexA];

			// Register all dynamic-dynamic pairs.
			for (uint32 indexB = indexA + 1; indexB < dynamicPool.componentCount; indexB += 1) {
				entityPair.B = dynamicPool.componentIndexToEntity[indexB];

				// ENGINE_DEBUG("Registered dynamic-dynamic pair ([], []).", pair.A, pair.B);

				entityPairs.emplace_back(entityPair);
			}

			// Register all dynamic-static pairs.
			for (uint32 indexB = 0; indexB < staticPool.componentCount; indexB += 1) {
				entityPair.B = staticPool.componentIndexToEntity[indexB];

				// ENGINE_DEBUG("Registered dynamic-static pair ([], []).", pair.A, pair.B);

				entityPairs.emplace_back(entityPair);
			}
		}

		return entityPairs;
	}

	std::vector<Arbiter2D> PhysicsEngine::NarrowPhase(
		const std::vector<EntityPair>& entityPairs
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) const {
		std::vector<Arbiter2D> arbiters;

		Arbiter2D arbiter;
		arbiter.scene = scene;

		for (const EntityPair& entityPair : entityPairs) {
			const PhysicalObjectCopy2D A = copyPhysicalObject(*scene, entityPair.A);
			const PhysicalObjectCopy2D B = copyPhysicalObject(*scene, entityPair.B);

			ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(entityPair.A));
			ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(entityPair.B));

			const ref<Material>& materialA = scene->getMaterial(entityPair.A);
			const ref<Material>& materialB = scene->getMaterial(entityPair.B);

			if (!COLLIDER_2D_TABLE[static_cast<uint32>(A.geometry->type)][static_cast<uint32>(B.geometry->type)](A, B, &arbiter.contactManifold, visualizationData)) {
				// Entities are not intersecting (or degenerate case).
				continue;
			}

			arbiter.A = A;
			arbiter.B = B;

			arbiters.emplace_back(arbiter);

			#ifdef BUILD_MODE_DEBUG
				if (visualizationData != nullptr) {
					visualizationData->contactManifold = arbiter.contactManifold;
				}
			#endif
		}

		return arbiters;
	}

	/* void PhysicsEngine::integrate() {
		for (Entity entity = 0; entity < scene->getPhysicalObjectEntityCount(); entity += 1) {
			const float32 t = safeAdvanceTimes[entity];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
			rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

			rigidBody->linearVelocity += (rigidBody->linearAcceleration + rigidBody->linearForces) * t * 0.5f;
			rigidBody->angularVelocity += (rigidBody->angularAcceleration + rigidBody->torque) * t * 0.5f;

			rigidBody->linearAcceleration = rigidBody->linearForces;
			rigidBody->angularAcceleration = rigidBody->torque;

			scene->updateTransform(entity);
		}

		for (const ImpulseData& impulseData : impulses) {
			const float32 toi = safeAdvanceTimes[impulseData.entity];

			// TODO: lyah::max
			// TODO: max(0.1, t) seems to be needed with 2 entities. Both don't work when gravity is enabled.
			// const float32 t = 1.0f - toi;
			const float32 t = std::max(0.1f, 1.0f - toi);

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(impulseData.entity));

			rigidBody->linearVelocity += impulseData.impulse * rigidBody->inverseMass;
			rigidBody->angularVelocity += area(impulseData.r, impulseData.impulse) * rigidBody->inverseInertia;

			rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
			rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

			rigidBody->linearVelocity += (rigidBody->linearAcceleration + rigidBody->linearForces) * t * 0.5f;
			rigidBody->angularVelocity += (rigidBody->angularAcceleration + rigidBody->torque) * t * 0.5f;

			rigidBody->linearAcceleration = rigidBody->linearForces;
			rigidBody->angularAcceleration = rigidBody->torque;

			scene->updateTransform(impulseData.entity);
		}
	} */
}