#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/PhysicsEngine2D.hpp"

namespace ProjectA {
	vec2<float32> PhysicsEngine2D::calculateRelativeVelocity(const RigidBody2D& A, const RigidBody2D& B, const Contact2D& contact) {
		return B.linearVelocity + cross(B.angularVelocity, contact.rB) - A.linearVelocity - cross(A.angularVelocity, contact.rA);
	}

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
			//       This looks like Velocity Verlet for gravity to me?
			// rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * deltaTime;
			// rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;
			rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * deltaTime;
		}

		// 4. Pre-compute step:
		//    - Calculate combined values in each pair (static/dynamic friction, restitution, etc.)
		//    - For each contact, calculate mass normal, mass tangent and bias value
		for (auto& entityContactPair : entityContactPairs) {
			precompute(entityContactPair.second, inverseDeltaTime);
		}

		// 5. Apply normal and friction impulses to overlapping pairs in a sequential manner.
		for (uint32 iteration = 0; iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT; iteration += 1) {
			for (auto& entityContactPair : entityContactPairs) {
				applyImpulse(entityContactPair.second);
			}
		}

		// 6. Integrate positions/rotations and velocities and reset forces.
		// TODO: Do we need to integrate positions/rotations on dynamic objects with an inverse mass of 0?
		// TODO: Do we need to integrate positions/rotations on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < dynamicPool.componentCount; componentIndex += 1) {
			const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];

			RigidBody2D& rigidBody = scene->getRigidBody(entity);

			// TODO: Exclude dynamic objects with an inverse mass of 0 from the loop.
			if (rigidBody.inverseMass == 0.0f) {
				continue;
			}

			rigidBody.position += rigidBody.linearVelocity * deltaTime;
			rigidBody.rotation += rigidBody.angularVelocity * deltaTime;

			rigidBody.updateTransform();

			// rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
			// rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;

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
			// NOTE: This step only occurs if the dynamic pool contains more than 1 component.
			for (uint32 indexB = indexA + 1; indexB < dynamicPool.componentCount; indexB += 1) {
				entityPair.B = dynamicPool.componentIndexToEntity[indexB];

				entityPairs.emplace_back(entityPair);
			}

			// Register all dynamic-static pairs.
			for (uint32 indexB = 0; indexB < staticPool.componentCount; indexB += 1) {
				entityPair.B = staticPool.componentIndexToEntity[indexB];

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

		for (const EntityPair entityPair : entityPairs) {
			const EntityPairKey key = entityPair.asKey();

			const RigidBody2D& rigidBodyA = scene->getRigidBody(entityPair.A);
			const RigidBody2D& rigidBodyB = scene->getRigidBody(entityPair.B);

			if (
				!COLLIDER_2D_TABLE[static_cast<uint32>(rigidBodyA.geometry->type)][static_cast<uint32>(rigidBodyB.geometry->type)](
					rigidBodyA,
					rigidBodyB,
					&entityContactPair.contactManifold
					#ifdef BUILD_MODE_DEBUG
						, visualizationData
					#endif
				)
			) {
				// Entities are not intersecting (or degenerate case).
				entityContactPairs.erase(key);

				continue;
			}

			std::unordered_map<EntityPairKey, EntityContactPair2D>::iterator entityContactPairIterator = entityContactPairs.find(key);

			if (entityContactPairIterator == entityContactPairs.end()) {
				entityContactPair.A = entityPair.A;
				entityContactPair.B = entityPair.B;

				entityContactPairs.insert(std::pair<EntityPairKey, EntityContactPair2D>(key, entityContactPair));
			}
			else {
				// TODO: This path seems never active.
				mergeContactManifolds(entityContactPairIterator->second, entityContactPair.contactManifold);
			}

			#ifdef BUILD_MODE_DEBUG
				if (visualizationData != nullptr) {
					visualizationData->contactManifold = entityContactPair.contactManifold;
				}
			#endif
		}
	}

	void PhysicsEngine2D::mergeContactManifolds(EntityContactPair2D& entityContactPair, const ContactManifold2D& contactManifold) {
		ContactManifold2D mergedContactManifold;

		for (uint32 contactIndex = 0; contactIndex < contactManifold.getContactCount(); contactIndex += 1) {
			const Contact2D& contact = contactManifold.contacts[contactIndex];

			mergedContactManifold.contacts[contactIndex] = contact;

			#ifdef WARM_STARTING
				for (uint32 previousContactIndex = 0; previousContactIndex < entityContactPair.contactManifold.getContactCount(); previousContactIndex += 1) {
					const Contact2D& previousContact = entityContactPair.contactManifold.contacts[previousContactIndex];

					if (contact == previousContact) {
						mergedContactManifold.contacts[contactIndex].accumulatedNormalImpulse = previousContact.accumulatedNormalImpulse;
						mergedContactManifold.contacts[contactIndex].accumulatedTangentImpulse = previousContact.accumulatedTangentImpulse;

						break;
					}
				}
			#endif
		}

		entityContactPair.contactManifold = mergedContactManifold;
		entityContactPair.contactManifold.setContactCount(contactManifold.getContactCount());
		entityContactPair.contactManifold.normal = contactManifold.normal;
	}

	void PhysicsEngine2D::precompute(EntityContactPair2D& pair, float32 inverseDeltaTime) {
		const RigidBody2D& rigidBodyA = scene->getRigidBody(pair.A);
		const RigidBody2D& rigidBodyB = scene->getRigidBody(pair.B);

		pair.contactManifold.tangent = perpendicular(pair.contactManifold.normal, 1.0f);
		pair.combinedFriction = lyah::sqrt(rigidBodyA.friction * rigidBodyB.friction);

		for (uint8 contactPointIndex = 0; contactPointIndex < pair.contactManifold.getContactCount(); contactPointIndex += 1) {
			Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

			// Distance from the contact to the shape's COM.
			contact.rA = contact.position - rigidBodyA.position;
			contact.rB = contact.position - rigidBodyB.position;

			const float32 inverseMassSum = rigidBodyA.inverseMass + rigidBodyB.inverseMass;

			// Precompute normal mass, tangent mass, and bias.
			const float32 rnA = dot(contact.rA, pair.contactManifold.normal);
			const float32 rnB = dot(contact.rB, pair.contactManifold.normal);

			const float32 massNormal =
				(dot(contact.rA, contact.rA) - rnA * rnA) * rigidBodyA.inverseInertia +
				(dot(contact.rB, contact.rB) - rnB * rnB) * rigidBodyB.inverseInertia +
				inverseMassSum;

			contact.massNormal = 1.0f / massNormal;

			const float32 rtA = dot(contact.rA, pair.contactManifold.tangent);
			const float32 rtB = dot(contact.rB, pair.contactManifold.tangent);
			const float32 massTangent =
				(dot(contact.rA, contact.rA) - rtA * rtA) * rigidBodyA.inverseInertia +
				(dot(contact.rB, contact.rB) - rtB * rtB) * rigidBodyB.inverseInertia +
				inverseMassSum;

			contact.massTangent = 1.0f / massTangent;

			contact.bias = -PENETRATION_BIAS * inverseDeltaTime * std::min(0.0f, contact.separation + ALLOWED_PENETRATION);

			#ifdef ACCUMULATE_IMPULSES
				RigidBody2D& rigidBodyA = scene->getRigidBody(pair.A);
				RigidBody2D& rigidBodyB = scene->getRigidBody(pair.B);

				const vec2<float32> impulse = pair.contactManifold.normal * contact.accumulatedNormalImpulse + pair.contactManifold.tangent * contact.accumulatedTangentImpulse;

				rigidBodyA.applyImpulse(-impulse, contact.rA);
				rigidBodyB.applyImpulse(impulse, contact.rA);
			#endif
		}
	}

	void PhysicsEngine2D::applyImpulse(EntityContactPair2D& pair) {
		RigidBody2D& rigidBodyA = scene->getRigidBody(pair.A);
		RigidBody2D& rigidBodyB = scene->getRigidBody(pair.B);
		vec2<float32> relativeVelocity;

		for (uint8 contactPointIndex = 0; contactPointIndex < pair.contactManifold.getContactCount(); contactPointIndex += 1) {
			Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

			// Calculate the relative velocity at the contact point.
			relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact);

			const float32 relativeVelocityTowardsNormal = dot(relativeVelocity, pair.contactManifold.normal);

			float32 normalImpulseLength = (-relativeVelocityTowardsNormal + contact.bias) * contact.massNormal;

			#ifdef ACCUMULATE_IMPULSES
				const float32 oldNormalValue = contact.accumulatedNormalImpulse;

				// TODO: lyah::max
				contact.accumulatedNormalImpulse = std::max(0.0f, contact.accumulatedNormalImpulse + normalImpulseLength);

				normalImpulseLength = contact.accumulatedNormalImpulse - oldNormalValue;
			#else
				// TODO: lyah::max
				normalImpulseLength = std::max(0.0f, normalImpulseLength);
			#endif

			const vec2<float32> normalImpulse = pair.contactManifold.normal * normalImpulseLength;

			// Apply normal impulse.
			rigidBodyA.applyImpulse(-normalImpulse, contact.rA);
			rigidBodyB.applyImpulse(normalImpulse, contact.rB);

			// Calculate the updated relative velocity at the contact point.
			relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact);

			const float32 relativeVelocityTowardsTangent = dot(relativeVelocity, pair.contactManifold.tangent);

			float32 tangentImpulseLength = -relativeVelocityTowardsTangent * contact.massTangent;

			#ifdef ACCUMULATE_IMPULSES
				const float32 maxTangentImpulseLength = pair.combinedFriction * contact.accumulatedNormalImpulse;
				const float32 oldTangentValue = contact.accumulatedTangentImpulse;

				// TODO: lyah::max
				contact.accumulatedTangentImpulse = std::clamp(contact.accumulatedTangentImpulse + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

				tangentImpulseLength = contact.accumulatedTangentImpulse - oldTangentValue;
			#else
				const float32 maxTangentImpulseLength = pair.combinedFriction * normalImpulseLength;

				// TODO: lyah::clamp
				tangentImpulseLength = std::clamp(tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);
			#endif

			const vec2<float32> tangentImpulse = pair.contactManifold.tangent * tangentImpulseLength;

			// Apply tangent (friction) impulse.
			rigidBodyA.applyImpulse(-tangentImpulse, contact.rA);
			rigidBodyB.applyImpulse(tangentImpulse, contact.rB);
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