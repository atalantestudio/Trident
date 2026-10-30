// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/PhysicsSimulation3D.hpp"

namespace Trident {
	inline static vec3<float32> calculateRelativeVelocity(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, vec3<float32> rA, vec3<float32> rB) {
		return rigidBodyB.linearVelocity + cross(rB, rigidBodyB.angularVelocity) - rigidBodyA.linearVelocity - cross(rA, rigidBodyA.angularVelocity);
	}

	PhysicsSimulation3D::PhysicsSimulation3D(const PhysicsSimulation3DDescriptor& descriptor) :
		scene(descriptor.scene),
		deltaTime(static_cast<float32>(descriptor.deltaTime)),
		inverseDeltaTime(static_cast<float32>(1.0 / descriptor.deltaTime)),
		playerEntity(descriptor.playerEntity)
	{}

	void PhysicsSimulation3D::tick() {
		rigidBodyPairs.clear();

		// TODO(debug)
		/* broadPhase();
		narrowPhase();
		applyForces();
		updateContactManifolds();
		applyImpulses();
		integrate(); */
	}

	void PhysicsSimulation3D::broadPhase() {
		const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

		RigidBodyPair3D pair;
		uint32 componentIndexA = 0;

		// TODO(physics/3d/broad-phase): Implement a better broad-phase.
		while (componentIndexA < rigidBodyComponentPool.componentCount) {
			const Entity A = rigidBodyComponentPool.componentIndexToEntity[componentIndexA];
			const Geometry3DComponent& geometryA = scene->ecs.getComponent<Geometry3DComponent>(A);

			if (!geometryA.hasHull) {
				componentIndexA += 1;

				continue;
			}

			pair.hullA = &geometryA.hull;
			pair.rigidBodyA = &scene->ecs.getComponent<RigidBody3DComponent>(A);

			uint32 componentIndexB = componentIndexA + 1;

			while (componentIndexB < rigidBodyComponentPool.componentCount) {
				const Entity B = rigidBodyComponentPool.componentIndexToEntity[componentIndexB];
				const Geometry3DComponent& geometryB = scene->ecs.getComponent<Geometry3DComponent>(B);

				if (!geometryB.hasHull) {
					componentIndexB += 1;

					continue;
				}

				pair.hullB = &geometryB.hull;
				pair.rigidBodyB = &scene->ecs.getComponent<RigidBody3DComponent>(B);

				rigidBodyPairs.emplace_back(pair);

				componentIndexB += 1;
			}

			componentIndexA += 1;
		}
	}

	void PhysicsSimulation3D::narrowPhase() {
		ContactPair3D contactPair;

		for (const RigidBodyPair3D& pair : rigidBodyPairs) {
			static constexpr Collider3D COLLISION_TABLE[Hull3DType::count][Hull3DType::count] = {
				{PolygonPolygon},
			};

			// The broad phase must ensure that A hull type < B hull type.
			const Collider3D collider = COLLISION_TABLE[pair.hullA->type][pair.hullB->type];

			if (!collider(pair, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				contactPairs.erase(pair);

				continue;
			}

			const std::map<const RigidBodyPair3D, ContactPair3D>::iterator contactPairIterator = contactPairs.find(pair);

			if (contactPairIterator == contactPairs.end()) {
				contactPairs.insert({pair, contactPair});

				continue;
			}

			mergeContactManifolds(contactPairIterator->second, contactPair.manifold);
		}
	}

	void PhysicsSimulation3D::applyForces() {
		ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();
		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			// TODO(physics/3d/apply-forces): Remove debug code.
			if (rigidBodyComponentPool.componentIndexToEntity[componentIndex] == playerEntity) {
				componentIndex += 1;

				continue;
			}

			rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + G) * deltaTime;

			componentIndex += 1;
		}
	}

	void PhysicsSimulation3D::updateContactManifolds() {
		const ref<Scene3DAsset> asset = scene->getAsset();

		for (std::pair<const RigidBodyPair3D, ContactPair3D>& pair : contactPairs) {
			const Hull3D& hullA = *pair.first.hullA;
			const Hull3D& hullB = *pair.first.hullB;

			RigidBody3DComponent& rigidBodyA = *pair.first.rigidBodyA;
			RigidBody3DComponent& rigidBodyB = *pair.first.rigidBodyB;

			ContactPair3D& contactPair = pair.second;

			// TODO(physics/3d/ground): Restore ground entity.
			/* if (dot(contactPair.manifold.normal, UP) > 0.0f) {
				rigidBodyB.groundEntity = contactPair.A;
			}
			else if (dot(contactPair.manifold.normal, -UP) > 0.0f) {
				rigidBodyA.groundEntity = contactPair.B;
			} */

			vec3<float32> normalAntiParallel = RIGHT;

			if (abs(contactPair.manifold.normal.x) > 0.9f) {
				normalAntiParallel = UP;
			}

			contactPair.manifold.tangent0 = normalized(cross(contactPair.manifold.normal, normalAntiParallel));
			contactPair.manifold.tangent1 = cross(contactPair.manifold.normal, contactPair.manifold.tangent0);
			contactPair.combinedFriction = sqrt(rigidBodyA.friction * rigidBodyB.friction);

			uint8 contactIndex = 0;

			while (contactIndex < contactPair.manifold.contactCount) {
				Contact3D& contact = contactPair.manifold.contacts[contactIndex];
				contact.rA = contact.position - rigidBodyA.position;
				contact.rB = contact.position - rigidBodyB.position;

				const float32 inverseMassSum = rigidBodyA.inverseMass + rigidBodyB.inverseMass;

				{
					const vec3<float32> rnA = cross(cross(contact.rA, contactPair.manifold.normal) * rigidBodyA.inverseInertia, contact.rA);
					const vec3<float32> rnB = cross(cross(contact.rB, contactPair.manifold.normal) * rigidBodyB.inverseInertia, contact.rB);
					const float32 denominator = inverseMassSum + dot(rnA + rnB, contactPair.manifold.normal);

					if (abs(denominator) < 0.001f) {
						contact.massNormal = 0.0f;
					}
					else {
						contact.massNormal = 1.0f / denominator;
					}
				}

				{
					const vec3<float32> rt0A = cross(cross(contact.rA, contactPair.manifold.tangent0) * rigidBodyA.inverseInertia, contact.rA);
					const vec3<float32> rt0B = cross(cross(contact.rB, contactPair.manifold.tangent0) * rigidBodyB.inverseInertia, contact.rB);
					const float32 denominator = inverseMassSum + dot(rt0A + rt0B, contactPair.manifold.tangent0);

					if (abs(denominator) < 0.001f) {
						contact.massTangent0 = 0.0f;
					}
					else {
						contact.massTangent0 = 1.0f / denominator;
					}
				}

				{
					const vec3<float32> rt1A = cross(cross(contact.rA, contactPair.manifold.tangent1) * rigidBodyA.inverseInertia, contact.rA);
					const vec3<float32> rt1B = cross(cross(contact.rB, contactPair.manifold.tangent1) * rigidBodyB.inverseInertia, contact.rB);
					const float32 denominator = inverseMassSum + dot(rt1A + rt1B, contactPair.manifold.tangent1);

					if (abs(denominator) < 0.001f) {
						contact.massTangent1 = 0.0f;
					}
					else {
						contact.massTangent1 = 1.0f / denominator;
					}
				}

				const float32 penetrationBias = (hullA.getPenetrationBias() + hullB.getPenetrationBias()) * 0.5f;

				contact.bias = -penetrationBias * inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
					const vec3<float32> j =
						contactPair.manifold.normal * contact.accumulatedNormalImpulse +
						contactPair.manifold.tangent0 * contact.accumulatedTangent0Impulse +
						contactPair.manifold.tangent1 * contact.accumulatedTangent1Impulse;

					rigidBodyA.applyImpulse(-j, contact.rA);
					rigidBodyB.applyImpulse(j, contact.rB);
				#endif

				contactIndex += 1;
			}
		}
	}

	void PhysicsSimulation3D::applyImpulses() {
		uint32 iteration = 0;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const RigidBodyPair3D, ContactPair3D>& pair : contactPairs) {
				RigidBody3DComponent& rigidBodyA = *pair.first.rigidBodyA;
				RigidBody3DComponent& rigidBodyB = *pair.first.rigidBodyB;
				ContactPair3D& contactPair = pair.second;
				uint8 contactIndex = 0;

				while (contactIndex < contactPair.manifold.contactCount) {
					Contact3D& contact = contactPair.manifold.contacts[contactIndex];

					float32 normalImpulseLength;

					// Apply normal impulse.
					{
						const vec3<float32> relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

						normalImpulseLength = (-dot(relativeVelocity, contactPair.manifold.normal) + contact.bias) * contact.massNormal;

						#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
							const float32 oldNormalValue = contact.accumulatedNormalImpulse;

							contact.accumulatedNormalImpulse = max(0.0f, oldNormalValue + normalImpulseLength);

							normalImpulseLength = contact.accumulatedNormalImpulse - oldNormalValue;
						#else
							normalImpulseLength = max(0.0f, normalImpulseLength);
						#endif

						const vec3<float32> j = contactPair.manifold.normal * normalImpulseLength;

						rigidBodyA.applyImpulse(-j, contact.rA);
						rigidBodyB.applyImpulse(j, contact.rB);
					}

					// Apply first friction impulse.
					{
						const vec3<float32> relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

						float32 tangentImpulseLength = -dot(relativeVelocity, contactPair.manifold.tangent0) * contact.massTangent0;

						#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * contact.accumulatedNormalImpulse;
							const float32 oldTangentValue = contact.accumulatedTangent0Impulse;

							contact.accumulatedTangent0Impulse = clamp(oldTangentValue + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

							tangentImpulseLength = contact.accumulatedTangent0Impulse - oldTangentValue;
						#else
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * normalImpulseLength;

							tangentImpulseLength = clamp(tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);
						#endif

						const vec3<float32> j = contactPair.manifold.tangent0 * tangentImpulseLength;

						rigidBodyA.applyImpulse(-j, contact.rA);
						rigidBodyB.applyImpulse(j, contact.rB);
					}

					// Apply second friction impulse.
					{
						const vec3<float32> relativeVelocity = calculateRelativeVelocity(rigidBodyA, rigidBodyB, contact.rA, contact.rB);

						float32 tangentImpulseLength = -dot(relativeVelocity, contactPair.manifold.tangent1) * contact.massTangent1;

						#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * contact.accumulatedNormalImpulse;
							const float32 oldTangentValue = contact.accumulatedTangent1Impulse;

							contact.accumulatedTangent1Impulse = clamp(oldTangentValue + tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);

							tangentImpulseLength = contact.accumulatedTangent1Impulse - oldTangentValue;
						#else
							const float32 maxTangentImpulseLength = contactPair.combinedFriction * normalImpulseLength;

							tangentImpulseLength = clamp(tangentImpulseLength, -maxTangentImpulseLength, maxTangentImpulseLength);
						#endif

						const vec3<float32> j = contactPair.manifold.tangent1 * tangentImpulseLength;

						rigidBodyA.applyImpulse(-j, contact.rA);
						rigidBodyB.applyImpulse(j, contact.rB);
					}

					contactIndex += 1;
				}
			}

			iteration += 1;
		}
	}

	void PhysicsSimulation3D::integrate() {
		const ref<Scene3DAsset> asset = scene->getAsset();

		ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();
		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const Geometry3DComponent& geometryComponent = scene->ecs.getComponent<Geometry3DComponent>(entity);

			rigidBodyComponent.position += rigidBodyComponent.linearVelocity * deltaTime;
			rigidBodyComponent.rotation += rigidBodyComponent.angularVelocity * deltaTime;
			rigidBodyComponent.updateOrientation();
			rigidBodyComponent.updateWorld(geometryComponent.hull.centroid);
			rigidBodyComponent.force = {0.0_N, 0.0_N, 0.0_N};

			componentIndex += 1;
		}

		scene->updateRigidBodyFacePlanes();
	}

	void PhysicsSimulation3D::mergeContactManifolds(ContactPair3D& contactPair, const ContactManifold3D& newContactManifold) {
		ContactManifold3D mergedContactManifold{};
		uint32 contactIndex = 0;

		while (contactIndex < newContactManifold.contactCount) {
			const Contact3D& cNew = newContactManifold.contacts[contactIndex];

			int k = -1;
			int j = 0;

			while (j < contactPair.manifold.contactCount) {
				const Contact3D& cOld = contactPair.manifold.contacts[j];

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

			#if defined(ENABLE_PHYSICS_3D_WARM_STARTING)
				const Contact3D& cOld = contactPair.manifold.contacts[k];

				mergedContactManifold.contacts[contactIndex].accumulatedNormalImpulse = cOld.accumulatedNormalImpulse;
				mergedContactManifold.contacts[contactIndex].accumulatedTangent0Impulse = cOld.accumulatedTangent0Impulse;
				mergedContactManifold.contacts[contactIndex].accumulatedTangent1Impulse = cOld.accumulatedTangent1Impulse;
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
}