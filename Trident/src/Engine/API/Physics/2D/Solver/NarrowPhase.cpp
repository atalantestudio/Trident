// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void mergeContactManifolds(ContactPair2D& contactPair, const ContactManifold2D& newContactManifold) {
		ContactManifold2D mergedContactManifold{};
		uint32 contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			const Contact2D& cNew = newContactManifold.contacts[contactIndex];

			sint32 k = -1;
			sint32 j = 0;

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

	void narrowPhase(PhysicsSimulation2D& simulation) {
		static constexpr uint32 SHAPE_TYPE_COUNT = static_cast<uint32>(ShapeType2D::count);
		static constexpr Collider2D COLLIDER_2D_TABLE[SHAPE_TYPE_COUNT][SHAPE_TYPE_COUNT] = {
			{CircleCircle2D, CircleCapsule2D,  CircleConvexHull2D    },
			{nullptr,        CapsuleCapsule2D, CapsuleConvexHull2D   },
			{nullptr,        nullptr,          ConvexHullConvexHull2D},
		};

		ContactPair2D contactPair;

		for (const CollisionPair2D collisionPair : simulation.collisionPairs) {
			const Shape2D& shapeA = simulation.shapes[collisionPair.shapeIdA];
			const Shape2D& shapeB = simulation.shapes[collisionPair.shapeIdB];

			const RigidBody2D& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
			const RigidBody2D& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

			// The broad phase must ensure that A hull type < B hull type.
			const Collider2D collider = COLLIDER_2D_TABLE[static_cast<uint32>(shapeA.type)][static_cast<uint32>(shapeB.type)];

			DEBUG_ASSERT(collider != nullptr);

			if (!collider(shapeA, shapeB, rigidBodyA, rigidBodyB, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				simulation.contactPairs.erase(collisionPair);

				continue;
			}

			const std::map<const CollisionPair2D, ContactPair2D>::iterator contactPairIterator = simulation.contactPairs.find(collisionPair);

			if (contactPairIterator == simulation.contactPairs.end()) {
				simulation.contactPairs.insert({collisionPair, contactPair});

				continue;
			}

			mergeContactManifolds(contactPairIterator->second, contactPair.manifold);
		}
	}
}