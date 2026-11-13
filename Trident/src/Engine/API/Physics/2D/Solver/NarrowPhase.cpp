// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void mergeContactManifolds(ContactManifold2D& manifold, const ContactManifold2D& newManifold) {
		#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
			Contact2D mergedContacts[2];
		#endif

		uint32 contactIndex = 0;

		while (contactIndex < newManifold.contactCount) {
			const Contact2D& cNew = newManifold.contacts[contactIndex];

			#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
				uint32 k = max<uint32>();
				uint32 j = 0;

				while (j < manifold.contactCount) {
					const Contact2D& cOld = manifold.contacts[j];

					if (cNew.id == cOld.id) {
						k = j;

						break;
					}

					j += 1;
				}

				Contact2D& c = mergedContacts[contactIndex];
			#else
				Contact2D& c = manifold.contacts[contactIndex];
			#endif

			c = cNew;

			#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
				if (k != max<uint32>()) {
					const Contact2D& cOld = manifold.contacts[k];

					c.accumulatedNormalImpulse = cOld.accumulatedNormalImpulse;
					c.accumulatedTangentImpulse = cOld.accumulatedTangentImpulse;
				}
			#else
				c.accumulatedNormalImpulse = 0.0f;
				c.accumulatedTangentImpulse = 0.0f;
			#endif

			contactIndex += 1;
		}

		#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
			contactIndex = 0;

			while (contactIndex < newManifold.contactCount) {
				manifold.contacts[contactIndex] = mergedContacts[contactIndex];

				contactIndex += 1;
			}
		#endif

		manifold.contactCount = newManifold.contactCount;
		manifold.normal = newManifold.normal;
	}

	void narrowPhase(PhysicsSimulation2D& simulation) {
		static constexpr uint32 SHAPE_TYPE_COUNT = static_cast<uint32>(ShapeType2D::count);
		static constexpr Collider2D COLLIDER_2D_TABLE[SHAPE_TYPE_COUNT][SHAPE_TYPE_COUNT] = {
			{CircleCircle2D, CircleCapsule2D,  CircleConvexHull2D    },
			{nullptr,        CapsuleCapsule2D, CapsuleConvexHull2D   },
			{nullptr,        nullptr,          ConvexHullConvexHull2D},
		};

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

			mergeContactManifolds(contactPairIterator->second.manifold, contactPair.manifold);
		}
	}
}