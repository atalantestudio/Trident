#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/PhysicsEngine2D.hpp"

namespace ProjectA {
	void PhysicsEngine2D::registerScene(Scene2D& scene) {
		this->scene = &scene;
	}

	void PhysicsEngine2D::tick(
		float32 deltaTime,
		float32 inverseDeltaTime
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	) {
		const float32 halfDeltaTime = deltaTime * 0.5f;

		const ComponentPool<DynamicEntityTag>& dynamicPool = scene->getDynamicEntityPool();

		entityPairs.clear();
		entityContactPairs.clear();

		// 1. Broad phase: Detect potentially overlapping pairs.
		// TODO: If less than 2 objects in the same layer AND in different physical groups, no chance of overlap.
		BroadPhase();

		// 2. Narrow phase: Calculate the contact manifold for each pair.
		NarrowPhase(
			#ifdef BUILD_MODE_DEBUG
				visualizationData
			#endif
		);

		// 3. Integrate forces.
		// TODO: Do we need to integrate forces on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < dynamicPool.componentCount; componentIndex += 1) {
			const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];

			RigidBody2D& rigidBody = scene->getRigidBody(entity);

			// TODO: Exclude dynamic objects with an inverse mass of 0 from the loop.
			if (rigidBody.inverseMass == 0.0f) {
				continue;
			}

			// NOTE: https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
			//       This seems like Velocity Verlet to me?
			// rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
			// rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;
			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * deltaTime;
			rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * deltaTime;
		}

		// 4. Pre-compute step:
		//    - Calculate combined values in each pair (static/dynamic friction, restitution, etc.)
		//    - For each contact, calculate mass normal, mass tangent and bias value
		for (EntityContactPair2D& entityContactPair : entityContactPairs) {
			precompute(entityContactPair, inverseDeltaTime);
		}

		// 5. Apply normal and friction impulses to overlapping pairs in a sequential manner.
		for (uint32 iteration = 0; iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT; iteration += 1) {
			for (EntityContactPair2D& entityContactPair : entityContactPairs) {
				applyImpulse(entityContactPair);
			}
		}

		// 6. Integrate positions/rotations and velocities and reset forces.
		// TODO: Do we need to integrate positions/rotations on dynamic objects with an inverse mass of 0?
		// TODO: Do we need to integrate positions/rotations on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < dynamicPool.componentCount; componentIndex += 1) {
			const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];

			RigidBody2D& rigidBody = scene->getRigidBody(entity);

			rigidBody.position += rigidBody.linearVelocity * deltaTime;
			rigidBody.rotation += rigidBody.angularVelocity * deltaTime;

			rigidBody.updateTransform();

			/* if (rigidBody->inverseMass > 0.0f) {
				rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
				rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;
			} */

			rigidBody.force = {0.0f, 0.0f};
			rigidBody.torque = 0.0f;
		}
	}

	void PhysicsEngine2D::BroadPhase() {
		const ComponentPool<DynamicEntityTag>& dynamicPool = scene->getDynamicEntityPool();
		const ComponentPool<StaticEntityTag>& staticPool = scene->getStaticEntityPool();

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
	}

	void PhysicsEngine2D::NarrowPhase(
		#ifdef BUILD_MODE_DEBUG
			VisualizationData2D* visualizationData
		#endif
	) {
		EntityContactPair2D entityContactPair;

		for (const EntityPair& entityPair : entityPairs) {
			const RigidBody2D& rigidBodyA = scene->getRigidBody(entityPair.A);
			const RigidBody2D& rigidBodyB = scene->getRigidBody(entityPair.B);

			if (!COLLIDER_2D_TABLE[static_cast<uint32>(rigidBodyA.geometry->type)][static_cast<uint32>(rigidBodyB.geometry->type)](rigidBodyA, rigidBodyB, &entityContactPair.contactManifold, visualizationData)) {
				// Entities are not intersecting (or degenerate case).
				continue;
			}

			entityContactPair.A = entityPair.A;
			entityContactPair.B = entityPair.B;

			entityContactPairs.emplace_back(entityContactPair);

			#ifdef BUILD_MODE_DEBUG
				if (visualizationData != nullptr) {
					visualizationData->contactManifold = entityContactPair.contactManifold;
				}
			#endif
		}
	}

	void PhysicsEngine2D::precompute(EntityContactPair2D& pair, float32 inverseDeltaTime) {
		static constexpr float32 ALLOWED_PENETRATION = 0.01f;
		static constexpr float32 PENETRATION_BIAS = true ? 0.2f : 0.0f;

		const RigidBody2D& rigidBodyA = scene->getRigidBody(pair.A);
		const RigidBody2D& rigidBodyB = scene->getRigidBody(pair.B);

		// pair.contactManifold.tangent = perpendicular(pair.contactManifold.normal, 1.0f);
		// pair.combinedFriction = lyah::sqrt(rigidBodyA.friction * rigidBodyB.friction);

		for (uint8 contactPointIndex = 0; contactPointIndex < pair.contactManifold.getContactCount(); contactPointIndex += 1) {
			Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

			// Distance from the contact to the shape's COM.
			contact.rA = contact.position - rigidBodyA.position;
			contact.rB = contact.position - rigidBodyB.position;

			// Precompute normal mass, tangent mass, and bias.
			const float32 rnA = dot(contact.rA, pair.contactManifold.normal);
			const float32 rnB = dot(contact.rB, pair.contactManifold.normal);

			const float32 massNormal =
				(dot(contact.rA, contact.rA) - rnA * rnA) * rigidBodyA.inverseInertia +
				(dot(contact.rB, contact.rB) - rnB * rnB) * rigidBodyB.inverseInertia +
				rigidBodyA.inverseMass +
				rigidBodyB.inverseMass;

			contact.massNormal = 1.0f / massNormal;

			/* const float32 rtA = dot(contact.rA, contactManifold.tangent);
			const float32 rtB = dot(contact.rB, contactManifold.tangent);
			const float32 massTangent =
				(dot(contact.rA, contact.rA) - rtA * rtA) * rigidBodyA.inverseInertia +
				(dot(contact.rB, contact.rB) - rtB * rtB) * rigidBodyB.inverseInertia +
				rigidBodyA.inverseMass +
				rigidBodyB.inverseMass;

			contact.massTangent = 1.0f / massTangent; */

			contact.bias = -PENETRATION_BIAS * inverseDeltaTime * std::min(0.0f, contact.separation + ALLOWED_PENETRATION);

			/* // Accumulate impulses.
			{
				// Apply normal + friction impulse.
				const vec2<float32> j = contact.accumulatedNormalImpulse * contactManifold.normal + contact.accumulatedTangentImpulse * tangent;

				rigidBodyA.applyImpulse(-j, rA);
				rigidBodyB.applyImpulse(j, rB);
			} */
		}
	}

	void PhysicsEngine2D::applyImpulse(const EntityContactPair2D& pair) {
		RigidBody2D& rigidBodyA = scene->getRigidBody(pair.A);
		RigidBody2D& rigidBodyB = scene->getRigidBody(pair.B);

		for (uint8 contactPointIndex = 0; contactPointIndex < pair.contactManifold.getContactCount(); contactPointIndex += 1) {
			const Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

			const vec2<float32> dv =
				rigidBodyB.linearVelocity + cross(rigidBodyB.angularVelocity, contact.rB) -
				rigidBodyA.linearVelocity - cross(rigidBodyA.angularVelocity, contact.rA);

			const float32 vn = dot(dv, pair.contactManifold.normal);

			// NOTE: Restitution gets applied here (0.0f).
			float32 normalImpulseLength = contact.massNormal * (-vn + contact.bias);

			// Clamp the accumulated impulse.
			/* {
				const float32 previousANI = contact.accumulatedNormalImpulse;

				contact.accumulatedNormalImpulse = std::max(0.0f, previousANI + normalImpulseLength);

				normalImpulseLength = contact.accumulatedNormalImpulse - previousANI;
			} */
			normalImpulseLength = std::max(0.0f, normalImpulseLength);

			const vec2<float32> normalImpulse = pair.contactManifold.normal * normalImpulseLength;

			// Apply normal impulse.
			rigidBodyA.applyImpulse(-normalImpulse, contact.rA);
			rigidBodyB.applyImpulse(normalImpulse, contact.rB);

			// TODO: Friction impulse.
			/* const float32 vt = dot(dv, contactManifold.tangent);
			float32 frictionImpulseLength = contact.massTangent * -vt;

			const float32 maxPt = combinedFriction * normalImpulseLength;

			// TODO: lyah::clamp
			frictionImpulseLength = std::clamp(frictionImpulseLength, -maxPt, maxPt);

			const vec2<float32> tangentImpulse = contactManifold.tangent * frictionImpulseLength;

			// Apply tangent impulse.
			rigidBodyA.applyImpulse(-tangentImpulse, contact.rA);
			rigidBodyB.applyImpulse(tangentImpulse, contact.rB); */
		}
	}

	/* void PhysicsEngine2D::integrate() {
		for (const Entity entity : entities) {
			const float32 t = safeAdvanceTimes[entity];

			RigidBody2D& rigidBody = scene->getRigidBody(entity);

			rigidBody.position += rigidBody.linearVelocity * t + rigidBody.linearAcceleration * t * t * 0.5f;
			rigidBody.rotation += rigidBody.angularVelocity * t + rigidBody.angularAcceleration * t * t * 0.5f;

			rigidBody.linearVelocity += (rigidBody.linearAcceleration + rigidBody.linearForces) * t * 0.5f;
			rigidBody.angularVelocity += (rigidBody.angularAcceleration + rigidBody.torque) * t * 0.5f;

			rigidBody.linearAcceleration = rigidBody.linearForces;
			rigidBody.angularAcceleration = rigidBody.torque;

			scene->updateTransform(entity);
		}

		for (const ImpulseData& impulseData : impulses) {
			const float32 toi = safeAdvanceTimes[impulseData.entity];

			// TODO: lyah::max
			// TODO: max(0.1, t) seems to be needed with 2 entities. Both don't work when gravity is enabled.
			// const float32 t = 1.0f - toi;
			const float32 t = std::max(0.1f, 1.0f - toi);

			RigidBody2D& rigidBody = scene->getRigidBody(impulseData.entity);

			rigidBody.linearVelocity += impulseData.impulse * rigidBody.inverseMass;
			rigidBody.angularVelocity += area(impulseData.r, impulseData.impulse) * rigidBody.inverseInertia;

			rigidBody.position += rigidBody.linearVelocity * t + rigidBody.linearAcceleration * t * t * 0.5f;
			rigidBody.rotation += rigidBody.angularVelocity * t + rigidBody.angularAcceleration * t * t * 0.5f;

			rigidBody.linearVelocity += (rigidBody.linearAcceleration + rigidBody.linearForces) * t * 0.5f;
			rigidBody.angularVelocity += (rigidBody.angularAcceleration + rigidBody.torque) * t * 0.5f;

			rigidBody.linearAcceleration = rigidBody.linearForces;
			rigidBody.angularAcceleration = rigidBody.torque;

			scene->updateTransform(impulseData.entity);
		}
	} */
}