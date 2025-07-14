#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/EPA2D.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/PhysicsEngine.hpp"

namespace ProjectA {
	void PhysicsEngine::setScene(Scene2D& scene) {
		this->scene = &scene;
	}

	void PhysicsEngine::tick(float32 deltaTime, GJK2DResult* gjkResult) {
		const float32 halfDeltaTime = deltaTime * 0.5f;

		float32 inverseDeltaTime = 0.0f;
		
		if (deltaTime > 0.0f) {
			inverseDeltaTime = 1.0f / deltaTime;
		}

		const ComponentPool<DynamicEntityComponent>& pool = scene->getDynamicEntityPool();

		// 1. Broad phase: Detect potentially overlapping pairs.
		// TODO: If less than 2 objects in the same layer AND in different physical groups, no chance of overlap.
		const std::vector<CollisionPair> collisionPairs = BroadPhase();

		// 2. Narrow phase: Calculate the contact manifold for each pair.
		std::vector<Arbiter2D> arbiters = NarrowPhase(collisionPairs, gjkResult);

		// 3. Integrate forces.
		// TODO: Do we need to integrate forces on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < pool.componentCount; componentIndex += 1) {
			const Entity entity = pool.componentIndexToEntity[componentIndex];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			// TODO: Exclude dynamic objects with an inverse mass of 0 from the loop.
			if (rigidBody->inverseMass == 0.0f) {
				continue;
			}

			// NOTE: https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
			//       This seems like Velocity Verlet to me?
			rigidBody->linearVelocity += (rigidBody->force * rigidBody->inverseMass + G) * halfDeltaTime;
			rigidBody->angularVelocity += rigidBody->torque * rigidBody->inverseInertia * halfDeltaTime;
		}

		// 4. Pre-compute step:
		//    - Calculate combined values in each pair (static/dynamic friction, restitution, etc.)
		//    - For each contact point, calculate mass normal, mass tangent and bias value
		// TODO
		for (Arbiter2D& arbiter : arbiters) {
			arbiter.precompute(inverseDeltaTime);
		}

		// 5. Apply normal and friction impulses to overlapping pairs in a sequential manner.
		// TODO
		for (uint32 iteration = 0; iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT; iteration += 1) {
			for (Arbiter2D& arbiter : arbiters) {
				arbiter.applyImpulse();
			}
		}

		// 6. Integrate positions/rotations and velocities and reset forces.
		// TODO: Do we need to integrate positions/rotations on dynamic objects with an inverse mass of 0?
		// TODO: Do we need to integrate positions/rotations on resting objects? E.g. they could be at rest on a moving platform due to inertia.
		for (uint32 componentIndex = 0; componentIndex < pool.componentCount; componentIndex += 1) {
			const Entity entity = pool.componentIndexToEntity[componentIndex];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			rigidBody->position += rigidBody->linearVelocity * deltaTime;
			rigidBody->rotation += rigidBody->angularVelocity * deltaTime;

			if (rigidBody->inverseMass > 0.0f) {
				rigidBody->linearVelocity += (rigidBody->force * rigidBody->inverseMass + G) * halfDeltaTime;
				rigidBody->angularVelocity += rigidBody->torque * rigidBody->inverseInertia * halfDeltaTime;
			}

			rigidBody->force = {0.0f, 0.0f};
			rigidBody->torque = 0.0f;

			scene->updateTransform(entity);
		}
	}

	std::vector<CollisionPair> PhysicsEngine::BroadPhase() const {
		std::vector<CollisionPair> collisionPairs;
		CollisionPair pair;

		// TODO: If less than 2 objects in the same layer AND in different physical groups, no chance of overlap.
		for (const std::vector<Entity>& layer : scene->getLayers()) {
			if (layer.size() < 2) {
				continue;
			}

			for (Entity A = 0; A < layer.size() - 1; A += 1) {
				pair.A = A;

				for (Entity B = A + 1; B < layer.size(); B += 1) {
					pair.B = B;

					collisionPairs.emplace_back(pair);
				}

				// TODO: Create dynamic-static pairs as well.
			}
		}

		return collisionPairs;
	}

	std::vector<Arbiter2D> PhysicsEngine::NarrowPhase(const std::vector<CollisionPair>& collisionPairs, GJK2DResult* gjkResultPointer) const {
		std::vector<Arbiter2D> arbiters;
		EPA2DResult epaResult;

		Arbiter2D arbiter;
		arbiter.scene = scene;

		for (const CollisionPair& pair : collisionPairs) {
			const PhysicalObjectCopy2D A = copyPhysicalObject(*scene, pair.A);
			const PhysicalObjectCopy2D B = copyPhysicalObject(*scene, pair.B);

			ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.A));
			ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.B));

			const ref<Material>& materialA = scene->getMaterial(pair.A);
			const ref<Material>& materialB = scene->getMaterial(pair.B);

			const float32 penetration = COLLIDER_PENETRATION_2D_TABLE[static_cast<uint32>(A.geometry->type)][static_cast<uint32>(B.geometry->type)](A, B);

			if (penetration > 0.0f) {
				// Entities are not intersecting (or degenerate case).
				continue;
			}

			arbiter.A = A;
			arbiter.B = B;
			arbiter.contactManifold = COLLIDER_2D_TABLE[static_cast<uint32>(A.geometry->type)][static_cast<uint32>(B.geometry->type)](A, B, penetration);
			arbiter.contactManifold.combinedFriction = lyah::sqrt(rigidBodyA->friction * rigidBodyB->friction);

			arbiters.emplace_back(arbiter);
		}

		return arbiters;
	}

	/* void PhysicsEngine::BroadPhase(GJK2DResult* gjkResult) {
		if (scene->getPhysicalObjectEntityCount() < 2) {
			return;
		}

		for (const std::vector<Entity>& layer : scene->getLayers()) {
			if (layer.size() < 2) {
				continue;
			}

			ref<RigidBody2D> rigidBodyA;
			ref<RigidBody2D> rigidBodyB;
			CollisionPair collisionPair;

			for (Entity A = 0; A < layer.size() - 1; A += 1) {
				rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A));
				collisionPair.A = A;

				for (Entity B = A + 1; B < layer.size(); B += 1) {
					rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B));

					if (rigidBodyA->inverseMass == 0.0f && rigidBodyB->inverseMass == 0.0f) {
						continue;
					}

					collisionPair.B = B;

					const ContactManifold2D contactManifold = GenerateContactManifold(A, B, gjkResult);

					if (contactManifold.contactPointCount == 0) {
						arbiters.erase(collisionPair);

						continue;
					}

					auto arbiterIterator = arbiters.find(collisionPair);

					if (arbiterIterator == arbiters.end()) {
						arbiterIterator->second.update(contactManifold);

						continue;
					}

					Arbiter2D arbiter;
					arbiter.scene = scene;
					arbiter.A = A;
					arbiter.B = B;
					arbiter.contactManifold = contactManifold;
					arbiter.contactManifold.combinedFriction = lyah::sqrt(rigidBodyA->friction * rigidBodyB->friction);

					arbiters.insert(std::pair<CollisionPair, Arbiter2D>(collisionPair, arbiter));
				}
			}
		}
	} */

	/* void PhysicsEngine::narrowPhase(GJK2DResult* gjkResult) {
		for (CollisionPair& pair : pairs) {
			TimeOfImpact output;

			output = evaluateContinuousCollision(pair.A, pair.B, gjkResult);
			// output.state = TimeOfImpactState::SEPARATED;
			// output.fraction = 1.0f;

			const float32 toi = output.fraction;

			// TODO: lyah::min
			safeAdvanceTimes[pair.A] = std::min(safeAdvanceTimes[pair.A], toi);
			safeAdvanceTimes[pair.B] = std::min(safeAdvanceTimes[pair.B], toi);

			// https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
			if (output.state == TimeOfImpactState::HIT) {
				const PhysicalObjectCopy2D copyA = copyPhysicalObject(*scene, pair.A, toi);
				const PhysicalObjectCopy2D copyB = copyPhysicalObject(*scene, pair.B, toi);

				ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.A));
				ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.B));
				const ref<Material>& materialA = scene->getMaterial(pair.A);
				const ref<Material>& materialB = scene->getMaterial(pair.B);

				const vec2<float32> n = normalized(copyB.position - copyA.position);
				vec2<float32> relativeLinearVelocity = copyB.linearVelocity - copyA.linearVelocity;
				const float32 velocityAlongNormal = dot(relativeLinearVelocity, n);

				if (velocityAlongNormal > 0.0f) {
					// These entities are separating; continue to the next entities.
					continue;
				}

				// TODO: lyah::min
				// Calculate coefficient of restitution.
				const float32 e = std::min(materialA->restitution, materialB->restitution);

				// Calculate linear impulse.
				// Fn is the magnitude of the collision normal.
				const float32 Fn = (-(1 + e) * velocityAlongNormal) / (rigidBodyA->inverseMass + rigidBodyB->inverseMass);
				const vec2<float32> normalImpulse = n * Fn;

				// TODO: Fix infinite tangent vector.
				// Linear velocity + normal impulse.
				// const vec2<float32> vA = copyA.linearVelocity - normalImpulse * rigidBodyA->inverseMass;
				// const vec2<float32> vB = copyB.linearVelocity + normalImpulse * rigidBodyB->inverseMass;

				// relativeLinearVelocity = vB - vA;

				// Tangent to the collision normal.
				// const vec2<float32> tangent = normalized(relativeLinearVelocity - n * dot(relativeLinearVelocity, n));

				// Magnitude of the friction.
				// const float32 Ff = -dot(relativeLinearVelocity, tangent) / (rigidBodyA->inverseMass + rigidBodyB->inverseMass);

				// const float32 mu = (rigidBodyA->staticFriction + rigidBodyB->staticFriction) * 0.5f;

				// vec2<float32> frictionImpulse = tangent * Ff;

				// if (abs(Ff) > mu * Fn) {
					// frictionImpulse = tangent * -Fn * ((rigidBodyA->dynamicFriction + rigidBodyB->dynamicFriction) * 0.5f);
				// }

				// Vector from the center of mass to the contact point on A.
				const vec2<float32> rA = output.closestA - copyA.geometry->centerOfMass;

				// Vector from the center of mass to the contact point on B.
				const vec2<float32> rB = output.closestB - copyB.geometry->centerOfMass;

				ImpulseData impulseDataA;
				impulseDataA.entity = pair.A;
				impulseDataA.impulse = -normalImpulse; // - frictionImpulse
				impulseDataA.r = rA;

				ImpulseData impulseDataB;
				impulseDataB.entity = pair.B;
				impulseDataB.impulse = +normalImpulse; // + frictionImpulse
				impulseDataB.r = rB;

				impulses.emplace_back(impulseDataA);
				impulses.emplace_back(impulseDataB);
			}
		}
	} */

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