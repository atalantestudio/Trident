// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Collider/Collider2DTable.hpp"
#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Physics/PhysicsEngine2D.hpp"

namespace Trident {
	#if 0
		inline static std::ostream& operator<<(std::ostream& stream, vec2<float32> argument) {
			stream << '(' << argument[0] << ", " << argument[1] << ')';

			return stream;
		}

		inline static void debugEntity(Entity entity, const RigidBody2D& rigidBody) {
			std::cout.precision(16);
			std::cout
				<< "Entity " << entity << "\n"
				"- Linear velocity = " << rigidBody.linearVelocity << "\n"
				"- Angular velocity = " << rigidBody.angularVelocity << "\n";
		}

		inline static void debugEntities(const Scene2D& scene) {
			const ComponentPool<DynamicEntityTag>& dynamicPool = scene.getComponentPool<DynamicEntityTag>();
			uint32 componentIndex = 0;

			while (componentIndex < dynamicPool.componentCount) {
				const Entity entity = dynamicPool.componentIndexToEntity[componentIndex];
				const RigidBody2D& rigidBody = scene.getRigidBody(entity);

				if (rigidBody.inverseMass > 0.0_kg) {
					debugEntity(entity, rigidBody);
				}

				componentIndex += 1;
			}
		}
	#endif

	inline vec2<float32> calculateRelativeVelocity(const RigidBody2D& A, const RigidBody2D& B, vec2<float32> rA, vec2<float32> rB) {
		return B.linearVelocity + cross(rB, -B.angularVelocity) - A.linearVelocity - cross(rA, -A.angularVelocity);
	}

	PhysicsEngine2D::PhysicsEngine2D(const PhysicsEngine2DDescriptor& descriptor) :
		deltaTime(static_cast<float32>(descriptor.deltaTime)),
		inverseDeltaTime(static_cast<float32>(1.0 / descriptor.deltaTime)),
		halfDeltaTime(static_cast<float32>(descriptor.deltaTime * 0.5))
		#if defined(BUILD_MODE_DEBUG)
			, visualizationData(descriptor.visualizationData)
		#endif
	{}

	void PhysicsEngine2D::registerScene(Scene2D& scene) {
		this->scene = &scene;
	}

	void PhysicsEngine2D::tick() {
		entityPairs.clear();

		BroadPhase();
		NarrowPhase();
		ApplyForces();
		UpdateContactManifolds();
		UpdateJoints();
		ApplyImpulses();
		Integrate();
	}

	void PhysicsEngine2D::BroadPhase() {
		const ComponentPool<DynamicEntityTag>& dynamicPool = scene->getComponentPool<DynamicEntityTag>();
		const ComponentPool<StaticEntityTag>& staticPool = scene->getComponentPool<StaticEntityTag>();

		EntityPair entityPair;
		uint32 indexA = 0;
		uint32 indexB;

		// TODO: Only register potentially overlapping entities.
		while (indexA < dynamicPool.componentCount) {
			entityPair.A = dynamicPool.componentIndexToEntity[indexA];

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < dynamicPool.componentCount) {
				entityPair.B = dynamicPool.componentIndexToEntity[indexB];

				entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < staticPool.componentCount) {
				entityPair.B = staticPool.componentIndexToEntity[indexB];

				entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}

	void PhysicsEngine2D::NarrowPhase() {
		EntityContactPair2D entityContactPair;

		for (const EntityPair entityPair : entityPairs) {
			const RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(entityPair.A);
			const RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(entityPair.B);

			Geometry2DType typeA = rigidBodyA.geometry->type;
			Geometry2DType typeB = rigidBodyB.geometry->type;

			if (typeA > typeB) {
				typeA = rigidBodyB.geometry->type;
				typeB = rigidBodyA.geometry->type;
			}

			if (
				!COLLIDER_2D_TABLE[static_cast<uint32>(typeA)][static_cast<uint32>(typeB)](
					rigidBodyA,
					rigidBodyB,
					&entityContactPair.contactManifold
					#if defined(BUILD_MODE_DEBUG)
						, visualizationData
					#endif
				)
			) {
				// Entities are not intersecting (or degenerate case).
				entityContactPairs.erase(entityPair);

				continue;
			}

			#if defined(BUILD_MODE_DEBUG)
				if (visualizationData != nullptr) {
					visualizationData->contactManifold = entityContactPair.contactManifold;
				}
			#endif

			std::map<const EntityPair, EntityContactPair2D>::iterator entityContactPairIterator = entityContactPairs.find(entityPair);

			if (entityContactPairIterator == entityContactPairs.end()) {
				entityContactPair.A = entityPair.A;
				entityContactPair.B = entityPair.B;

				entityContactPairs.insert(std::pair<const EntityPair, EntityContactPair2D>(entityPair, entityContactPair));

				continue;
			}

			mergeContactManifolds(entityContactPairIterator->second, entityContactPair.contactManifold);
		}
	}

	void PhysicsEngine2D::ApplyForces() {
		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();
		uint32 componentIndex = 0;

		// TODO: Integrate velocities on dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (componentIndex < rigidBodyPool.componentCount) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[componentIndex];

			RigidBody2D& rigidBody = scene->getComponent<RigidBody2D>(entity);

			// TODO: Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBody.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			// TODO
			// Integrate gravity like in https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html.
			// This looks like Velocity Verlet for gravity to me?
			// rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * deltaTime;
			// rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;
			rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * deltaTime;

			componentIndex += 1;
		}
	}

	void PhysicsEngine2D::UpdateContactManifolds() {
		uint8 contactPointIndex;

		// Calculate combined values in each pair (static/dynamic friction, restitution, etc.).
		// For each contact, calculate mass normal, mass tangent and bias value.
		for (std::pair<const EntityPair, EntityContactPair2D>& entityContactPair : entityContactPairs) {
			EntityContactPair2D& pair = entityContactPair.second;

			RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(pair.A);
			RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(pair.B);

			pair.contactManifold.tangent = perpendicularRight(pair.contactManifold.normal);
			pair.combinedFriction = sqrt(rigidBodyA.friction * rigidBodyB.friction);

			contactPointIndex = 0;

			while (contactPointIndex < pair.contactManifold.contactCount) {
				Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

				// Distance from the contact to the entity's COM.
				contact.rA = contact.position - rigidBodyA.position;
				contact.rB = contact.position - rigidBodyB.position;

				const float32 inverseMassSum = rigidBodyA.inverseMass + rigidBodyB.inverseMass;

				// Precompute normal mass, tangent mass and bias.
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

				const float32 penetrationBias = avg(rigidBodyA.geometry->getPenetrationBias(), rigidBodyB.geometry->getPenetrationBias());

				contact.bias = -penetrationBias * inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ACCUMULATE_IMPULSES)
					const vec2<float32> impulse = pair.contactManifold.normal * contact.accumulatedNormalImpulse + pair.contactManifold.tangent * contact.accumulatedTangentImpulse;

					rigidBodyA.applyImpulse(-impulse, contact.rA);
					rigidBodyB.applyImpulse(impulse, contact.rB);
				#endif

				contactPointIndex += 1;
			}
		}
	}

	void PhysicsEngine2D::UpdateJoints() {
		const ComponentPool<Joint2D>& jointPool = scene->getComponentPool<Joint2D>();
		uint32 componentIndex = 0;

		while (componentIndex < jointPool.componentCount) {
			const Entity entity = jointPool.componentIndexToEntity[componentIndex];

			Joint2D& joint = scene->getComponent<Joint2D>(entity);
			RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(joint.A);
			RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(joint.B);

			// TODO: Why is the transpose needed?
			const mat2x2<float32> RAT = transpose(mat2x2<float32>::rotation(rigidBodyA.rotation));
			const mat2x2<float32> RBT = transpose(mat2x2<float32>::rotation(rigidBodyB.rotation));

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

			#if defined(WARM_STARTING)
				rigidBodyA.applyImpulse(-joint.accumulatedImpulse, joint.rA);
				rigidBodyB.applyImpulse(joint.accumulatedImpulse, joint.rB);
			#else
				joint.accumulatedImpulse = {0.0f, 0.0f};
			#endif

			componentIndex += 1;
		}
	}

	void PhysicsEngine2D::ApplyImpulses() {
		const ComponentPool<ref<Constraint2D>>& constraintPool = scene->getComponentPool<ref<Constraint2D>>();
		const ComponentPool<Joint2D>& jointPool = scene->getComponentPool<Joint2D>();
		vec2<float32> relativeVelocity;
		uint32 iteration = 0;
		uint8 contactPointIndex;
		uint32 constraintComponentIndex;
		uint32 jointComponentIndex;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const EntityPair, EntityContactPair2D>& entityContactPair : entityContactPairs) {
				EntityContactPair2D& pair = entityContactPair.second;
				RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(pair.A);
				RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(pair.B);

				contactPointIndex = 0;

				while (contactPointIndex < pair.contactManifold.contactCount) {
					Contact2D& contact = pair.contactManifold.contacts[contactPointIndex];

					// Calculate the relative velocity at the contact point.
					relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

					const float32 relativeVelocityTowardsNormal = dot(relativeVelocity, pair.contactManifold.normal);

					float32 normalImpulseLength = (-relativeVelocityTowardsNormal + contact.bias) * contact.massNormal;

					#if defined(ACCUMULATE_IMPULSES)
						const float32 oldNormalValue = contact.accumulatedNormalImpulse;

						contact.accumulatedNormalImpulse = max(0.0f, oldNormalValue + normalImpulseLength);

						normalImpulseLength = contact.accumulatedNormalImpulse - oldNormalValue;
					#else
						normalImpulseLength = max(0.0f, normalImpulseLength);
					#endif

					const vec2<float32> normalImpulse = pair.contactManifold.normal * normalImpulseLength;

					// Apply normal impulse.
					rigidBodyA.applyImpulse(-normalImpulse, contact.rA);
					rigidBodyB.applyImpulse(normalImpulse, contact.rB);

					// Calculate the updated relative velocity at the contact point.
					relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

					const float32 relativeVelocityTowardsTangent = dot(relativeVelocity, pair.contactManifold.tangent);

					float32 tangentImpulseLength = -relativeVelocityTowardsTangent * contact.massTangent;

					#if defined(ACCUMULATE_IMPULSES)
						const float32 maxTangentImpulseLength = pair.combinedFriction * contact.accumulatedNormalImpulse;
						const float32 oldTangentValue = contact.accumulatedTangentImpulse;

						contact.accumulatedTangentImpulse = clamp(oldTangentValue + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

						tangentImpulseLength = contact.accumulatedTangentImpulse - oldTangentValue;
					#else
						const float32 maxTangentImpulseLength = pair.combinedFriction * normalImpulseLength;

						tangentImpulseLength = clamp(tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);
					#endif

					const vec2<float32> tangentImpulse = pair.contactManifold.tangent * tangentImpulseLength;

					// Apply tangent (friction) impulse.
					rigidBodyA.applyImpulse(-tangentImpulse, contact.rA);
					rigidBodyB.applyImpulse(tangentImpulse, contact.rB);

					contactPointIndex += 1;
				}
			}

			constraintComponentIndex = 0;

			while (constraintComponentIndex < constraintPool.componentCount) {
				const Entity entity = constraintPool.componentIndexToEntity[constraintComponentIndex];
				const ref<Constraint2D> constraint = scene->getComponent<ref<Constraint2D>>(entity);

				constraint->update(scene, deltaTime);

				constraintComponentIndex += 1;
			}

			jointComponentIndex = 0;

			while (jointComponentIndex < jointPool.componentCount) {
				const Entity entity = jointPool.componentIndexToEntity[jointComponentIndex];

				Joint2D& joint = scene->getComponent<Joint2D>(entity);
				RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(joint.A);
				RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(joint.B);

				relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, joint.rA, joint.rB);

				const vec2<float32> j = (joint.bias - relativeVelocity - joint.softness * joint.accumulatedImpulse) * joint.M;

				joint.accumulatedImpulse += j;

				rigidBodyA.applyImpulse(-j, joint.rA);
				rigidBodyB.applyImpulse(j, joint.rB);

				jointComponentIndex += 1;
			}

			iteration += 1;
		}
	}

	void PhysicsEngine2D::Integrate() {
		const ComponentPool<RigidBody2D>& rigidBodyPool = scene->getComponentPool<RigidBody2D>();
		uint32 componentIndex = 0;

		// TODO: Integration for static entities (e.g. a moving platform that isn't affected by other forces)?
		// TODO: Integration for dynamic entities with infinite mass?
		// TODO: Integration for dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (componentIndex < rigidBodyPool.componentCount) {
			const Entity entity = rigidBodyPool.componentIndexToEntity[componentIndex];

			RigidBody2D& rigidBody = scene->getComponent<RigidBody2D>(entity);

			// TODO: Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBody.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			rigidBody.position += rigidBody.linearVelocity * deltaTime;
			rigidBody.rotation += rigidBody.angularVelocity * deltaTime;

			rigidBody.updateTransform();

			// TODO
			// rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + G) * halfDeltaTime;
			// rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * halfDeltaTime;

			rigidBody.force = {0.0_N, 0.0_N};
			rigidBody.torque = 0.0_Nm;

			componentIndex += 1;
		}
	}

	void PhysicsEngine2D::mergeContactManifolds(EntityContactPair2D& entityContactPair, const ContactManifold2D& newContactManifold) {
		ContactManifold2D mergedContactManifold{};
		uint32 contactIndex = 0;
		uint32 previousContactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			const Contact2D& cNew = newContactManifold.contacts[contactIndex];

			int k = -1;
			int j = 0;

			while (j < entityContactPair.contactManifold.contactCount) {
				const Contact2D& cOld = entityContactPair.contactManifold.contacts[j];

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

			#if defined(WARM_STARTING)
				const Contact2D& cOld = entityContactPair.contactManifold.contacts[k];

				mergedContactManifold.contacts[contactIndex].accumulatedNormalImpulse = cOld.accumulatedNormalImpulse;
				mergedContactManifold.contacts[contactIndex].accumulatedTangentImpulse = cOld.accumulatedTangentImpulse;
			#endif

			contactIndex += 1;
		}

		contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			entityContactPair.contactManifold.contacts[contactIndex] = mergedContactManifold.contacts[contactIndex];

			contactIndex += 1;
		}

		entityContactPair.contactManifold.contactCount = newContactManifold.contactCount;
		entityContactPair.contactManifold.normal = newContactManifold.normal;
	}
}