// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2DTable.hpp"
#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Physics/PhysicsSimulation2D.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	inline vec2<float32> calculateRelativeVelocity(const RigidBody2DComponent& A, const RigidBody2DComponent& B, vec2<float32> rA, vec2<float32> rB) {
		return B.linearVelocity + crossLength(rB, B.angularVelocity) - A.linearVelocity - crossLength(rA, A.angularVelocity);
	}

	void PhysicsSimulation2D::mergeContactManifolds(ContactPair2D& contactPair, const ContactManifold2D& newContactManifold) {
		ContactManifold2D mergedContactManifold{};
		uint32 contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			const Contact2D& cNew = newContactManifold.contacts[contactIndex];

			int k = -1;
			int j = 0;

			while (j < contactPair.manifold.contactCount) {
				const Contact2D& cOld = contactPair.manifold.contacts[j];

				if (cNew.id == cOld.id) {
					k = j;

					break;
				}

				j += 1;
			}

			if (k == -1) {
				mergedContactManifold.contacts[contactIndex] = newContactManifold.contacts[contactIndex];

				contactIndex += 1;

				continue;
			}

			mergedContactManifold.contacts[contactIndex] = cNew;

			#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
				const Contact2D& cOld = contactPair.manifold.contacts[k];

				mergedContactManifold.contacts[contactIndex].accumulatedNormalImpulse = cOld.accumulatedNormalImpulse;
				mergedContactManifold.contacts[contactIndex].accumulatedTangentImpulse = cOld.accumulatedTangentImpulse;
			#endif

			contactIndex += 1;
		}

		contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			contactPair.manifold.contacts[contactIndex] = mergedContactManifold.contacts[contactIndex];

			contactIndex += 1;
		}

		contactPair.manifold.contactCount = newContactManifold.contactCount;
		contactPair.manifold.normal = newContactManifold.normal;
	}

	PhysicsSimulation2D::PhysicsSimulation2D(const Descriptor<PhysicsSimulation2D>& descriptor) :
		scene(descriptor.scene),
		deltaTime(static_cast<float32>(descriptor.deltaTime)),
		inverseDeltaTime(static_cast<float32>(1.0 / descriptor.deltaTime))

		// TODO(physics/2d/delta-time): Use half delta-time.
		// halfDeltaTime(static_cast<float32>(descriptor.deltaTime * 0.5))
	{}

	void PhysicsSimulation2D::tick() {
		entityPairs.clear();

		broadPhase();
		narrowPhase();
		applyForces();
		updateContactManifolds();

		// TODO(physics/2d/joint): Restore joints.
		// updateJoints();

		applyImpulses();
		integrate();
	}

	const Hull2D& PhysicsSimulation2D::getHull(Entity entity) const {
		const view<Hull2D>& hulls = scene->getHulls();
		const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(entity);
		const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];

		ENGINE_ASSERT(geometry.hullIndex < hulls.count(), "The geometry doesn't have a hull.");

		return hulls[geometry.hullIndex];
	}

	void PhysicsSimulation2D::broadPhase() {
		const TagPool<DynamicEntityTag>& dynamicPool = scene->ecs.getTagPool<DynamicEntityTag>();
		const TagPool<StaticEntityTag>& staticPool = scene->ecs.getTagPool<StaticEntityTag>();

		EntityPair entityPair;
		uint32 indexA = 0;
		uint32 indexB;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < dynamicPool.tagCount) {
			entityPair.A = dynamicPool.tagIndexToEntity[indexA];

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < dynamicPool.tagCount) {
				entityPair.B = dynamicPool.tagIndexToEntity[indexB];

				entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < staticPool.tagCount) {
				entityPair.B = staticPool.tagIndexToEntity[indexB];

				entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}

	void PhysicsSimulation2D::narrowPhase() {
		ContactPair2D contactPair;

		for (const EntityPair entityPair : entityPairs) {
			const RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(entityPair.A);
			const RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(entityPair.B);
			const Hull2D& hullA = getHull(entityPair.A);
			const Hull2D& hullB = getHull(entityPair.B);

			Hull2DType typeA = hullA.type;
			Hull2DType typeB = hullB.type;

			if (typeA > typeB) {
				typeA = hullB.type;
				typeB = hullA.type;
			}

			const Collider2D collider = COLLIDER_2D_TABLE[static_cast<uint32>(typeA)][static_cast<uint32>(typeB)];

			if (!collider(rigidBodyA, rigidBodyB, hullA, hullB, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				contactPairs.erase(entityPair);

				continue;
			}

			const std::map<const EntityPair, ContactPair2D>::iterator contactPairIterator = contactPairs.find(entityPair);

			if (contactPairIterator == contactPairs.end()) {
				contactPair.A = entityPair.A;
				contactPair.B = entityPair.B;

				contactPairs.insert({entityPair, contactPair});

				continue;
			}

			mergeContactManifolds(contactPairIterator->second, contactPair.manifold);
		}
	}

	void PhysicsSimulation2D::applyForces() {
		// TODO(physics/2d/apply-forces): Implement torque.
		constexpr float32 torque = 0.0_Nm;

		ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		uint32 componentIndex = 0;

		// TODO(physics/2d/apply-forces): Integrate velocities on dynamic resting entities (e.g. an object on a moving platform may move due to inertia).
		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			// TODO(physics/2d/apply-forces): Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			// TODO(physics/2d/apply-forces)
			// Integrate gravity like in https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html.
			// This looks like Velocity Verlet for gravity to me?
			// rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + G) * halfDeltaTime;
			rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + G) * deltaTime;
			// rigidBodyComponent.angularVelocity += rigidBodyComponent.inverseInertia * halfDeltaTime;
			rigidBodyComponent.angularVelocity += torque * rigidBodyComponent.inverseInertia * deltaTime;

			componentIndex += 1;
		}
	}

	void PhysicsSimulation2D::updateContactManifolds() {
		// Calculate combined values in each pair (static/dynamic friction, restitution, etc.).
		// For each contact, calculate mass normal, mass tangent and bias value.
		for (std::pair<const EntityPair, ContactPair2D>& pair : contactPairs) {
			ContactPair2D& contactPair = pair.second;
			RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(contactPair.A);
			RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(contactPair.B);

			const Hull2D& hullA = getHull(contactPair.A);
			const Hull2D& hullB = getHull(contactPair.B);

			contactPair.manifold.tangent = perpendicularRight(contactPair.manifold.normal);
			contactPair.combinedFriction = sqrt(rigidBodyA.friction * rigidBodyB.friction);

			uint8 contactIndex = 0;

			while (contactIndex < contactPair.manifold.contactCount) {
				Contact2D& contact = contactPair.manifold.contacts[contactIndex];
				contact.rA = contact.position - rigidBodyA.position;
				contact.rB = contact.position - rigidBodyB.position;

				const float32 inverseMassSum = rigidBodyA.inverseMass + rigidBodyB.inverseMass;
				const float32 rArA = dot(contact.rA, contact.rA);
				const float32 rBrB = dot(contact.rB, contact.rB);

				{
					const float32 rnA = dot(contact.rA, contactPair.manifold.normal);
					const float32 rnB = dot(contact.rB, contactPair.manifold.normal);

					contact.massNormal = 1.0f / ((rArA - rnA * rnA) * rigidBodyA.inverseInertia + (rBrB - rnB * rnB) * rigidBodyB.inverseInertia + inverseMassSum);
				}

				{
					const float32 rtA = dot(contact.rA, contactPair.manifold.tangent);
					const float32 rtB = dot(contact.rB, contactPair.manifold.tangent);

					contact.massTangent = 1.0f / ((rArA - rtA * rtA) * rigidBodyA.inverseInertia + (rBrB - rtB * rtB) * rigidBodyB.inverseInertia + inverseMassSum);
				}

				const float32 penetrationBias = (hullA.getPenetrationBias() + hullB.getPenetrationBias()) * 0.5f;

				contact.bias = -penetrationBias * inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION)
					const vec2<float32> j = contactPair.manifold.normal * contact.accumulatedNormalImpulse + contactPair.manifold.tangent * contact.accumulatedTangentImpulse;

					rigidBodyA.applyImpulse(-j, contact.rA);
					rigidBodyB.applyImpulse(j, contact.rB);
				#endif

				contactIndex += 1;
			}
		}
	}

	void PhysicsSimulation2D::updateJoints() {
		const ComponentPool<Joint2D>& jointPool = scene->ecs.getComponentPool<Joint2D>();
		uint32 componentIndex = 0;

		while (componentIndex < jointPool.componentCount) {
			const Entity entity = jointPool.componentIndexToEntity[componentIndex];

			Joint2D& joint = scene->ecs.getComponent<Joint2D>(entity);
			RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(joint.A);
			RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(joint.B);

			// TODO(physics/2d/update-joints): Why is the transpose needed?
			const mat2x2<float32> RAT = transpose(rotationAngle<mat2x2<float32>>(rigidBodyA.rotation));
			const mat2x2<float32> RBT = transpose(rotationAngle<mat2x2<float32>>(rigidBodyB.rotation));

			joint.rA = joint.localAnchorA * RAT;
			joint.rB = joint.localAnchorB * RBT;

			const vec2<float32> pA = rigidBodyA.position + joint.rA;
			const vec2<float32> pB = rigidBodyB.position + joint.rB;
			const vec2<float32> pAB = pB - pA;

			const mat2x2<float32> K1 = {
				rigidBodyA.inverseMass + rigidBodyB.inverseMass, 0.0f,
				0.0f,                                            rigidBodyA.inverseMass + rigidBodyB.inverseMass,
			};
			const mat2x2<float32> K2 = {
				 rigidBodyA.inverseInertia * joint.rA[1] * joint.rA[1], -rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[1],
				-rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[1],  rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[0],
			};
			const mat2x2<float32> K3 = {
				 rigidBodyB.inverseInertia * joint.rB[1] * joint.rB[1], -rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[1],
				-rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[1],  rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[0],
			};
			const mat2x2<float32> S = {
				joint.softness, 0.0f,
				0.0f,           joint.softness,
			};
			const mat2x2<float32> K = K1 + K2 + K3 + S;

			joint.M = inverse(K);
			joint.bias = pAB * inverseDeltaTime * -joint.biasFactor;

			#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
				rigidBodyA.applyImpulse(-joint.accumulatedImpulse, joint.rA);
				rigidBodyB.applyImpulse(joint.accumulatedImpulse, joint.rB);
			#else
				joint.accumulatedImpulse = {0.0f, 0.0f};
			#endif

			componentIndex += 1;
		}
	}

	void PhysicsSimulation2D::applyImpulses() {
		// const ComponentPool<ref<Constraint2D>>& constraintPool = scene->ecs.getComponentPool<ref<Constraint2D>>();
		// const ComponentPool<Joint2D>& jointPool = scene->ecs.getComponentPool<Joint2D>();
		vec2<float32> relativeVelocity;
		uint32 iteration = 0;
		// uint32 constraintComponentIndex;
		// uint32 jointComponentIndex;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const EntityPair, ContactPair2D>& pair : contactPairs) {
				ContactPair2D& contactPair = pair.second;
				RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(contactPair.A);
				RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(contactPair.B);

				uint8 contactIndex = 0;

				while (contactIndex < contactPair.manifold.contactCount) {
					Contact2D& contact = contactPair.manifold.contacts[contactIndex];

					relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

					float32 normalImpulseLength;

					// Apply normal impulse.
					{
						const float32 relativeVelocityTowardsNormal = dot(relativeVelocity, contactPair.manifold.normal);

						normalImpulseLength = (-relativeVelocityTowardsNormal + contact.bias) * contact.massNormal;

						#if defined(ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION)
							const float32 oldNormalValue = contact.accumulatedNormalImpulse;

							contact.accumulatedNormalImpulse = max(0.0f, oldNormalValue + normalImpulseLength);

							normalImpulseLength = contact.accumulatedNormalImpulse - oldNormalValue;
						#else
							normalImpulseLength = max(0.0f, normalImpulseLength);
						#endif

						const vec2<float32> j = contactPair.manifold.normal * normalImpulseLength;

						rigidBodyA.applyImpulse(-j, contact.rA);
						rigidBodyB.applyImpulse(j, contact.rB);
					}

					relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

					// Apply friction impulse.
					{
						const float32 relativeVelocityTowardsTangent = dot(relativeVelocity, contactPair.manifold.tangent);

						float32 tangentImpulseLength = -relativeVelocityTowardsTangent * contact.massTangent;

						#if defined(ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION)
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * contact.accumulatedNormalImpulse;
							const float32 oldTangentValue = contact.accumulatedTangentImpulse;

							contact.accumulatedTangentImpulse = clamp(oldTangentValue + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

							tangentImpulseLength = contact.accumulatedTangentImpulse - oldTangentValue;
						#else
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * normalImpulseLength;

							tangentImpulseLength = clamp(tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);
						#endif

						const vec2<float32> j = contactPair.manifold.tangent * tangentImpulseLength;

						rigidBodyA.applyImpulse(-j, contact.rA);
						rigidBodyB.applyImpulse(j, contact.rB);
					}

					contactIndex += 1;
				}
			}

			// TODO(physics/2d/constraint): Restore constraints.
			/* constraintComponentIndex = 0;

			while (constraintComponentIndex < constraintPool.componentCount) {
				const Entity entity = constraintPool.componentIndexToEntity[constraintComponentIndex];
				const ref<Constraint2D> constraint = scene->ecs.getComponent<ref<Constraint2D>>(entity);

				constraint->update(&scene->ecs, deltaTime);

				constraintComponentIndex += 1;
			} */

			// TODO(physics/2d/joint): Restore joints.
			/* jointComponentIndex = 0;

			while (jointComponentIndex < jointPool.componentCount) {
				const Entity entity = jointPool.componentIndexToEntity[jointComponentIndex];

				Joint2D& joint = scene->ecs.getComponent<Joint2D>(entity);
				RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(joint.A);
				RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(joint.B);

				relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, joint.rA, joint.rB);

				const vec2<float32> j = (joint.bias - relativeVelocity - joint.softness * joint.accumulatedImpulse) * joint.M;

				joint.accumulatedImpulse += j;

				rigidBodyA.applyImpulse(-j, joint.rA);
				rigidBodyB.applyImpulse(j, joint.rB);

				jointComponentIndex += 1;
			} */

			iteration += 1;
		}
	}

	void PhysicsSimulation2D::integrate() {
		ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		uint32 componentIndex = 0;

		// TODO(physics/2d/integration): Integration for static entities (e.g. a moving platform that isn't affected by other forces)?
		// TODO(physics/2d/integration): Integration for dynamic entities with infinite mass?
		// TODO(physics/2d/integration): Integration for dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			// TODO(physics/2d/integration): Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			const Hull2D& hull = getHull(rigidBodyComponentPool.componentIndexToEntity[componentIndex]);

			rigidBodyComponent.position += rigidBodyComponent.linearVelocity * deltaTime;
			rigidBodyComponent.rotation += rigidBodyComponent.angularVelocity * deltaTime;
			rigidBodyComponent.updateWorld(hull.centroid);

			// TODO(physics/2d/integration)
			// rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + vec2<float32>(0.0f, G)) * halfDeltaTime;
			// rigidBodyComponent.angularVelocity += rigidBodyComponent.inverseInertia * halfDeltaTime;

			rigidBodyComponent.force = {0.0_N, 0.0_N};

			componentIndex += 1;
		}
	}
}