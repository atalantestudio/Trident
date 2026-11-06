// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	inline static vec2<float32> calculateRelativeVelocity(const RigidBody2D& A, const RigidBody2D& B, vec2<float32> rA, vec2<float32> rB) {
		return B.linearVelocity + perpendicularRight(rB) * B.angularVelocity - A.linearVelocity - perpendicularRight(rA) * A.angularVelocity;
	}

	void applyImpulses(PhysicsSimulation2D& simulation) {
		// TODO(physics/2d/constraint): Restore constraints.
		// const ComponentPool<ref<Constraint2D>>& constraintPool = scene->ecs.getComponentPool<ref<Constraint2D>>();

		// TODO(physics/2d/joint): Restore joints.
		// const ComponentPool<Joint2D>& jointPool = scene->ecs.getComponentPool<Joint2D>();

		vec2<float32> relativeVelocity;
		uint32 iteration = 0;

		// TODO(physics/2d/constraint): Restore constraints.
		// uint32 constraintComponentIndex;

		// TODO(physics/2d/joint): Restore joints.
		// uint32 jointComponentIndex;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const CollisionPair2D, ContactPair2D>& pair : simulation.contactPairs) {
				ContactPair2D& contactPair = pair.second;

				const Shape2D& shapeA = simulation.shapes[pair.first.shapeIdA];
				const Shape2D& shapeB = simulation.shapes[pair.first.shapeIdB];

				RigidBody2D& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
				RigidBody2D& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

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

						applyImpulse(rigidBodyA, -j, contact.rA);
						applyImpulse(rigidBodyB, j, contact.rB);
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

						applyImpulse(rigidBodyA, -j, contact.rA);
						applyImpulse(rigidBodyB, j, contact.rB);
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

				applyImpulse(rigidBodyA, -j, joint.rA);
				applyImpulse(rigidBodyB, j, joint.rB);

				jointComponentIndex += 1;
			} */

			iteration += 1;
		}
	}
}