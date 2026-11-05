// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	static void mergeContactManifolds(ContactPair2D& contactPair, const ContactManifold2D& newContactManifold) {
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

	void narrowPhase(PhysicsSimulation2D& simulation) {
		static constexpr Collider2D COLLIDER_2D_TABLE[MAX_HULL_2D_TYPE_COUNT][MAX_HULL_2D_TYPE_COUNT] = {
			{nullptr,                      nullptr,                      nullptr                     },
			{nullptr,                      CircleCircleCollider,         CircleConvexHullCollider    },
			{nullptr,                      nullptr,                      ConvexHullConvexHullCollider},
		};

		ContactPair2D contactPair;

		for (const CollisionPair2D collisionPair : simulation.collisionPairs) {
			const Hull2D& hullA = getHull(simulation, collisionPair.shapeIdA);
			const Hull2D& hullB = getHull(simulation, collisionPair.shapeIdB);

			const RigidBody2DComponent& rigidBodyA = simulation.scene->ecs.getComponent<RigidBody2DComponent>(collisionPair.shapeIdA);
			const RigidBody2DComponent& rigidBodyB = simulation.scene->ecs.getComponent<RigidBody2DComponent>(collisionPair.shapeIdB);

			// The broad phase must ensure that A hull type < B hull type.
			const Collider2D collider = COLLIDER_2D_TABLE[static_cast<uint32>(hullA.type)][static_cast<uint32>(hullB.type)];

			DEBUG_ASSERT(collider != nullptr);

			if (!collider(rigidBodyA, rigidBodyB, hullA, hullB, &contactPair.manifold)) {
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