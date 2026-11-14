// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/2D/Constraint/Constraint.hpp"
#include "Engine/API/Physics/2D/Joint.hpp"
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

	static vec2<float32> calculateRelativeVelocity(const RigidBody2DComponent& A, const RigidBody2DComponent& B, vec2<float32> rA, vec2<float32> rB) {
		return B.linearVelocity + perpendicularRight(rB) * B.angularVelocity - A.linearVelocity - perpendicularRight(rA) * A.angularVelocity;
	}

	static void mergeContactManifolds(ContactManifold2D& manifold, const ContactManifold2D& newManifold) {
		#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
			Contact2D mergedContacts[2]{};
			uint32 newContactIndex = 0;

			while (newContactIndex < newManifold.contactCount) {
				const Contact2D& cNew = newManifold.contacts[newContactIndex];
				const Contact2D* cOld = nullptr;

				uint32 contactIndex = 0;

				while (contactIndex < manifold.contactCount) {
					const Contact2D& c = manifold.contacts[contactIndex];

					if (cNew.id == c.id) {
						cOld = &c;

						break;
					}

					contactIndex += 1;
				}

				Contact2D& c = mergedContacts[newContactIndex];
				c = cNew;

				if (cOld != nullptr) {
					c.accumulatedNormalImpulse = cOld->accumulatedNormalImpulse;
					c.accumulatedTangentImpulse = cOld->accumulatedTangentImpulse;
				}

				newContactIndex += 1;
			}

			std::copy_n(mergedContacts, newManifold.contactCount, manifold.contacts);

			manifold.normal = newManifold.normal;
			manifold.contactCount = newManifold.contactCount;
		#else
			manifold = newManifold;
		#endif
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

		simulation.rigidBodies = sequence<RigidBody2DComponent>(staticPool.tagCount + dynamicPool.tagCount);
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

			const RigidBodyID2D rigidBodyId = createRigidBody(simulation, rigidBodyDescriptor);

			Hull2D shapeDescriptor = getHull(descriptor.scene, rigidBodyId);
			shapeDescriptor.rigidBodyId = rigidBodyId;

			createShape(simulation, shapeDescriptor);

			tagIndex += 1;
		}

		tagIndex = 0;

		while (tagIndex < dynamicPool.tagCount) {
			const Entity entity = dynamicPool.tagIndexToEntity[tagIndex];

			RigidBody2DComponent rigidBodyDescriptor = descriptor.scene->ecs.getComponent<RigidBody2DComponent>(entity);
			rigidBodyDescriptor.type = RigidBodyType::DYNAMIC;

			const RigidBodyID2D rigidBodyId = createRigidBody(simulation, rigidBodyDescriptor);

			Hull2D shapeDescriptor = getHull(descriptor.scene, rigidBodyId);
			shapeDescriptor.rigidBodyId = rigidBodyId;

			createShape(simulation, shapeDescriptor);

			tagIndex += 1;
		}

		simulation.collisionPairs = {};
		simulation.contactPairs = {};

		return simulation;
	}

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBody2DComponent& descriptor) {
		DEBUG_ASSERT(!std::isinf(descriptor.position.x) && !std::isnan(descriptor.position.x) && !std::isinf(descriptor.position.y) && !std::isnan(descriptor.position.y));
		DEBUG_ASSERT(!std::isinf(descriptor.rotation) && !std::isnan(descriptor.rotation));

		ENGINE_ASSERT(simulation.usedRigidBodyCount < simulation.rigidBodies.count(), "Rigid body count limit already reached.");

		simulation.rigidBodies[simulation.usedRigidBodyCount] = descriptor;

		const uint32 id = simulation.usedRigidBodyCount;

		simulation.usedRigidBodyCount += 1;

		return id;
	}

	ShapeID2D createShape(PhysicsSimulation2D& simulation, const Hull2D& descriptor) {
		// DEBUG_ASSERT(!std::isinf(descriptor.density) && !std::isnan(descriptor.density) && descriptor.density >= 0.0f);
		// DEBUG_ASSERT(!std::isinf(descriptor.material.friction) && !std::isnan(descriptor.material.friction) && descriptor.material.friction >= 0.0f);

		ENGINE_ASSERT(simulation.usedShapeCount < simulation.shapes.count(), "Shape count limit already reached.");
		ENGINE_ASSERT(descriptor.rigidBodyId < simulation.usedRigidBodyCount, "Invalid rigid body ID.");

		simulation.shapes[simulation.usedShapeCount] = descriptor;

		const RigidBody2DComponent& rigidBody = simulation.rigidBodies[descriptor.rigidBodyId];
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

	void broadPhase(PhysicsSimulation2D& simulation) {
		CollisionPair2D collisionPair;
		uint32 indexA = 0;
		uint32 indexB;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < simulation.broadPhase.usedDynamicShapeIdCount) {
			collisionPair.shapeIdA = simulation.broadPhase.dynamicShapeIds[indexA];

			indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < simulation.broadPhase.usedStaticShapeIdCount) {
				collisionPair.shapeIdB = simulation.broadPhase.staticShapeIds[indexB];

				simulation.collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				collisionPair.shapeIdB = simulation.broadPhase.dynamicShapeIds[indexB];

				simulation.collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}

	void narrowPhase(PhysicsSimulation2D& simulation) {
		static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Hull2DType::count)][static_cast<uint32>(Hull2DType::count)] = {
			{nullptr, nullptr, nullptr},
			{nullptr, CircleCircle2D, CircleConvexHull2D},
			{nullptr, nullptr, ConvexHullConvexHull2D},
		};

		for (const CollisionPair2D collisionPair : simulation.collisionPairs) {
			const Hull2D& shapeA = simulation.shapes[collisionPair.shapeIdA];
			const Hull2D& shapeB = simulation.shapes[collisionPair.shapeIdB];

			const RigidBody2DComponent& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
			const RigidBody2DComponent& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

			// The broad phase must ensure that A hull type < B hull type.
			const Collider2D collider = COLLIDER_2D_TABLE[static_cast<uint32>(shapeA.type)][static_cast<uint32>(shapeB.type)];

			DEBUG_ASSERT(collider != nullptr);

			ContactPair2D contactPair;

			if (!collider(shapeA, shapeB, rigidBodyA, rigidBodyB, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				simulation.contactPairs.erase(collisionPair);

				continue;
			}

			const std::map<CollisionPair2D, ContactPair2D>::iterator contactPairIterator = simulation.contactPairs.find(collisionPair);

			if (contactPairIterator == simulation.contactPairs.end()) {
				simulation.contactPairs.insert({collisionPair, contactPair});

				continue;
			}

			mergeContactManifolds(contactPairIterator->second.manifold, contactPair.manifold);
		}
	}

	void integrateVelocities(PhysicsSimulation2D& simulation) {
		// TODO(physics/2d/apply-forces): Implement torque.
		constexpr float32 torque = 0.0_Nm;

		uint32 shapeIdIndex = 0;

		// TODO(physics/2d/apply-forces): Integrate velocities on dynamic resting entities (e.g. an object on a moving platform may move due to inertia).
		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Hull2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2DComponent& rigidBody = simulation.rigidBodies[shape.rigidBodyId];
			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + simulation.gravity) * simulation.deltaTime;
			rigidBody.angularVelocity += torque * rigidBody.inverseInertia * simulation.deltaTime;

			shapeIdIndex += 1;
		}
	}

	void updateContactManifolds(PhysicsSimulation2D& simulation) {
		for (std::pair<const CollisionPair2D, ContactPair2D>& pair : simulation.contactPairs) {
			const Hull2D& shapeA = simulation.shapes[pair.first.shapeIdA];
			const Hull2D& shapeB = simulation.shapes[pair.first.shapeIdB];

			RigidBody2DComponent& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
			RigidBody2DComponent& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

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

				const float32 penetrationBias = (shapeA.getPenetrationBias() + shapeB.getPenetrationBias()) * 0.5f;

				contact.bias = -penetrationBias * simulation.inverseDeltaTime * min(0.0f, contact.separation + simulation.allowedPenetration);

				#if defined(ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION)
					const vec2<float32> j = contactPair.manifold.normal * contact.accumulatedNormalImpulse + contactPair.manifold.tangent * contact.accumulatedTangentImpulse;

					rigidBodyA.applyImpulse(-j, contact.rA);
					rigidBodyB.applyImpulse(j, contact.rB);
				#endif

				contactIndex += 1;
			}
		}
	}

	/* void updateJoints(PhysicsSimulation2D& simulation) {
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

	void applyImpulses(PhysicsSimulation2D& simulation) {
		// const ComponentPool<ref<Constraint2D>>& constraintPool = scene->ecs.getComponentPool<ref<Constraint2D>>();
		// const ComponentPool<Joint2D>& jointPool = scene->ecs.getComponentPool<Joint2D>();
		uint32 iteration = 0;

		while (iteration < simulation.impulseIterationCount) {
			for (std::pair<const CollisionPair2D, ContactPair2D>& pair : simulation.contactPairs) {
				const Hull2D& shapeA = simulation.shapes[pair.first.shapeIdA];
				const Hull2D& shapeB = simulation.shapes[pair.first.shapeIdB];

				RigidBody2DComponent& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
				RigidBody2DComponent& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

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

	void integratePositions(PhysicsSimulation2D& simulation) {
		uint32 shapeIdIndex = 0;

		// TODO(physics/2d/integration): Integration for static entities (e.g. a moving platform that isn't affected by other forces)?
		// TODO(physics/2d/integration): Integration for dynamic entities with infinite mass?
		// TODO(physics/2d/integration): Integration for dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Hull2D& hull = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2DComponent& rigidBody = simulation.rigidBodies[hull.rigidBodyId];
			rigidBody.position += rigidBody.linearVelocity * simulation.deltaTime;
			rigidBody.rotation += rigidBody.angularVelocity * simulation.deltaTime;
			rigidBody.updateWorld(hull.centroid);

			rigidBody.force = {0.0_N, 0.0_N};

			shapeIdIndex += 1;
		}
	}
}