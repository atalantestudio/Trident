// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Physics/PhysicsSimulation2D.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	inline vec2<float32> calculateRelativeVelocity(const RigidBody2DComponent& A, const RigidBody2DComponent& B, vec2<float32> rA, vec2<float32> rB) {
		return B.linearVelocity + perpendicularRight(rB) * B.angularVelocity - A.linearVelocity - perpendicularRight(rA) * A.angularVelocity;
	}

	void PhysicsSimulation2D::mergeContactManifolds(ContactPair2D& contactPair, const ContactManifold2D& newContactManifold) {
		ContactManifold2D mergedContactManifold{};
		uint32 contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			const Contact2D& cNew = newContactManifold.contacts[contactIndex];

			uint32 k = max<uint32>();
			sint32 j = 0;

			while (j < contactPair.manifold.contactCount) {
				const Contact2D& cOld = contactPair.manifold.contacts[j];

				if (cNew.id == cOld.id) {
					k = j;

					break;
				}

				j += 1;
			}

			if (k == max<uint32>()) {
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

	PhysicsSimulation2D::PhysicsSimulation2D(const PhysicsSimulation2DDescriptor& descriptor) :
		scene(descriptor.scene),
		deltaTime(static_cast<float32>(descriptor.deltaTime)),
		inverseDeltaTime(static_cast<float32>(1.0 / descriptor.deltaTime))
	{}

	void PhysicsSimulation2D::tick() {
		clearPairs();
		broadPhase();
		narrowPhase();
		integrateVelocities();
		updateContactManifolds();

		// TODO(physics/2d/joint): Restore joints.
		// updateJoints();

		applyImpulses();
		integratePositions();
	}

	void PhysicsSimulation2D::clearPairs() {
		collisionPairs.clear();
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

		CollisionPair2D collisionPair;
		uint32 indexA = 0;
		uint32 indexB;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < dynamicPool.tagCount) {
			collisionPair.shapeIdA = dynamicPool.tagIndexToEntity[indexA];

			indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < staticPool.tagCount) {
				collisionPair.shapeIdB = staticPool.tagIndexToEntity[indexB];

				collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < dynamicPool.tagCount) {
				collisionPair.shapeIdB = dynamicPool.tagIndexToEntity[indexB];

				collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}

	void PhysicsSimulation2D::narrowPhase() {
		static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Hull2DType::count)][static_cast<uint32>(Hull2DType::count)] = {
			{nullptr, nullptr, nullptr},
			{nullptr, CircleCircle2D, CircleConvexHull2D},
			{nullptr, nullptr, ConvexHullConvexHull2D},
		};

		for (const CollisionPair2D collisionPair : collisionPairs) {
			const RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(collisionPair.shapeIdA);
			const RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(collisionPair.shapeIdB);

			const Hull2D& hullA = getHull(collisionPair.shapeIdA);
			const Hull2D& hullB = getHull(collisionPair.shapeIdB);

			// The broad phase must ensure that A hull type < B hull type.
			const Collider2D collider = COLLIDER_2D_TABLE[static_cast<uint32>(hullA.type)][static_cast<uint32>(hullB.type)];

			DEBUG_ASSERT(collider != nullptr);

			ContactPair2D contactPair;

			if (!collider(hullA, hullB, rigidBodyA, rigidBodyB, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				contactPairs.erase(collisionPair);

				continue;
			}

			const std::map<CollisionPair2D, ContactPair2D>::iterator contactPairIterator = contactPairs.find(collisionPair);

			if (contactPairIterator == contactPairs.end()) {
				contactPairs.insert({collisionPair, contactPair});

				continue;
			}

			mergeContactManifolds(contactPairIterator->second, contactPair.manifold);
		}
	}

	void PhysicsSimulation2D::integrateVelocities() {
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

			rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + G) * deltaTime;
			rigidBodyComponent.angularVelocity += torque * rigidBodyComponent.inverseInertia * deltaTime;

			componentIndex += 1;
		}
	}

	void PhysicsSimulation2D::updateContactManifolds() {
		for (std::pair<const CollisionPair2D, ContactPair2D>& pair : contactPairs) {
			RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(pair.first.shapeIdA);
			RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(pair.first.shapeIdB);

			const Hull2D& hullA = getHull(pair.first.shapeIdA);
			const Hull2D& hullB = getHull(pair.first.shapeIdB);

			ContactPair2D& contactPair = pair.second;
			contactPair.friction = sqrt(rigidBodyA.friction * rigidBodyB.friction);
			contactPair.manifold.tangent = perpendicularRight(contactPair.manifold.normal);

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

					// TODO(physics/2d): The 2d_stacking instability was accidentally fixed by removing the outer parentheses and moving `inverseMassSum` to the left. Why?
					// const float32 normal = (rigidBodyA.inverseInertia * (rArA - rnA * rnA) + rigidBodyB.inverseInertia * (rBrB - rnB * rnB)) + inverseMassSum;
					const float32 normal = inverseMassSum + rigidBodyA.inverseInertia * (rArA - rnA * rnA) + rigidBodyB.inverseInertia * (rBrB - rnB * rnB);

					contact.massNormal = 1.0f / normal;
				}

				{
					const float32 rtA = dot(contact.rA, contactPair.manifold.tangent);
					const float32 rtB = dot(contact.rB, contactPair.manifold.tangent);
					const float32 tangent = inverseMassSum + rigidBodyA.inverseInertia * (rArA - rtA * rtA) + rigidBodyB.inverseInertia * (rBrB - rtB * rtB);

					contact.massTangent = 1.0f / tangent;
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

	/* void PhysicsSimulation2D::updateJoints() {
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
	} */

	void PhysicsSimulation2D::applyImpulses() {
		// const ComponentPool<ref<Constraint2D>>& constraintPool = scene->ecs.getComponentPool<ref<Constraint2D>>();
		// const ComponentPool<Joint2D>& jointPool = scene->ecs.getComponentPool<Joint2D>();
		uint32 iteration = 0;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const CollisionPair2D, ContactPair2D>& pair : contactPairs) {
				RigidBody2DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody2DComponent>(pair.first.shapeIdA);
				RigidBody2DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody2DComponent>(pair.first.shapeIdB);

				ContactPair2D& contactPair = pair.second;

				uint8 contactIndex = 0;

				while (contactIndex < contactPair.manifold.contactCount) {
					Contact2D& contact = contactPair.manifold.contacts[contactIndex];

					vec2<float32> relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);
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
							const float32 maxTangentImpulseLength = contactPair.friction * contact.accumulatedNormalImpulse;
							const float32 oldTangentValue = contact.accumulatedTangentImpulse;

							contact.accumulatedTangentImpulse = clamp(oldTangentValue + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

							tangentImpulseLength = contact.accumulatedTangentImpulse - oldTangentValue;
						#else
							const float32 maxTangentImpulseLength = contactPair.friction * normalImpulseLength;

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
			/* uint32 constraintComponentIndex = 0;

			while (constraintComponentIndex < constraintPool.componentCount) {
				const Entity entity = constraintPool.componentIndexToEntity[constraintComponentIndex];
				const ref<Constraint2D> constraint = scene->ecs.getComponent<ref<Constraint2D>>(entity);

				constraint->update(&scene->ecs, deltaTime);

				constraintComponentIndex += 1;
			} */

			// TODO(physics/2d/joint): Restore joints.
			/* uint32 jointComponentIndex = 0;

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

	void PhysicsSimulation2D::integratePositions() {
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

			rigidBodyComponent.force = {0.0_N, 0.0_N};

			componentIndex += 1;
		}
	}
}