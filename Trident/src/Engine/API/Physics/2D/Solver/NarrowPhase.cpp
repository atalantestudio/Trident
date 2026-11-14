// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
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

	void narrowPhase(PhysicsSimulation2D& simulation) {
		static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Hull2DType::count)][static_cast<uint32>(Hull2DType::count)] = {
			{nullptr, nullptr, nullptr},
			{nullptr, CircleCircle2D, CircleConvexHull2D},
			{nullptr, nullptr, ConvexHullConvexHull2D},
		};

		for (const CollisionPair2D collisionPair : simulation.collisionPairs) {
			const Hull2D& shapeA = simulation.shapes[collisionPair.shapeIdA];
			const Hull2D& shapeB = simulation.shapes[collisionPair.shapeIdB];

			const RigidBody2D& rigidBodyA = simulation.rigidBodies[shapeA.rigidBodyId];
			const RigidBody2D& rigidBodyB = simulation.rigidBodies[shapeB.rigidBodyId];

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
}