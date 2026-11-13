// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static float32 getPenetrationBias(const Shape2D& shape) {
		switch (shape.type) {
			case ShapeType2D::CIRCLE: {
				return 0.01f;
			}
			case ShapeType2D::CAPSULE: {
				// TODO(physics/2d/shape/capsule)
				ENGINE_ERROR_UNIMPLEMENTED();

				return 0.0f;
			}
			case ShapeType2D::CONVEX_HULL: {
				return 0.2f;
			}
			default: {
				DEBUG_ASSERT(false);

				return 0.0f;
			}
		}
	}

	void updateContactManifolds(PhysicsSimulation2D& simulation) {
		for (std::pair<const CollisionPair2D, ContactPair2D>& pair : simulation.contactPairs) {
			const Shape2D& shapeA = simulation.shapes[pair.first.shapeIdA];
			const Shape2D& shapeB = simulation.shapes[pair.first.shapeIdB];

			RigidBody2D& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
			RigidBody2D& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

			ContactPair2D& contactPair = pair.second;
			contactPair.manifold.tangent = perpendicularRight(contactPair.manifold.normal);
			contactPair.combinedFriction = sqrt(shapeA.material.friction * shapeB.material.friction);

			uint8 contactIndex = 0;

			while (contactIndex < contactPair.manifold.contactCount) {
				Contact2D& contact = contactPair.manifold.contacts[contactIndex];
				contact.rA = contact.position - rigidBodyA.transform.position;
				contact.rB = contact.position - rigidBodyB.transform.position;

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

				const float32 penetrationBias = (getPenetrationBias(shapeA) + getPenetrationBias(shapeB)) * 0.5f;

				contact.bias = -penetrationBias * simulation.inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION)
					const vec2<float32> j = contactPair.manifold.normal * contact.accumulatedNormalImpulse + contactPair.manifold.tangent * contact.accumulatedTangentImpulse;

					applyImpulse(rigidBodyA, -j, contact.rA);
					applyImpulse(rigidBodyB, j, contact.rB);
				#endif

				contactIndex += 1;
			}
		}
	}
}