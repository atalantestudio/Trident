// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/PhysicsSimulation3D.hpp"

namespace Trident {
	static constexpr Collider3D COLLIDER_TABLE[static_cast<uint32>(Hull3DType::count)][static_cast<uint32>(Hull3DType::count)] = {
		{PolygonPolygon},
	};

	inline static vec3<float32> calculateRelativeVelocity(const RigidBody3DComponent& rigidBodyA, const RigidBody3DComponent& rigidBodyB, vec3<float32> rA, vec3<float32> rB) {
		return rigidBodyB.linearVelocity + cross(rB, rigidBodyB.angularVelocity) - rigidBodyA.linearVelocity - cross(rA, rigidBodyA.angularVelocity);
	}

	PhysicsSimulation3D::PhysicsSimulation3D(const Descriptor<PhysicsSimulation3D>& descriptor) :
		scene(descriptor.scene),
		deltaTime(static_cast<float32>(descriptor.deltaTime)),
		inverseDeltaTime(static_cast<float32>(1.0 / descriptor.deltaTime))
	{}

	void PhysicsSimulation3D::tick() {
		entityPairs.clear();

		broadPhase();
		narrowPhase();
		applyForces();
		updateContactManifolds();
		applyImpulses();
		integrate();
	}

	void PhysicsSimulation3D::broadPhase() {
		const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

		EntityPair entityPair;
		uint32 componentIndexA = 0;

		// TODO(physics/3d/broad-phase): Implement a better broad-phase.
		while (componentIndexA < rigidBodyComponentPool.componentCount) {
			entityPair.A = rigidBodyComponentPool.componentIndexToEntity[componentIndexA];

			uint32 componentIndexB = componentIndexA + 1;

			while (componentIndexB < rigidBodyComponentPool.componentCount) {
				entityPair.B = rigidBodyComponentPool.componentIndexToEntity[componentIndexB];

				entityPairs.emplace_back(entityPair);

				componentIndexB += 1;
			}

			componentIndexA += 1;
		}
	}

	void PhysicsSimulation3D::narrowPhase() {
		ContactPair3D contactPair;

		for (const EntityPair entityPair : entityPairs) {
			const RigidBody3DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody3DComponent>(entityPair.A);
			const RigidBody3DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody3DComponent>(entityPair.B);

			const GeometryComponent geometryComponentA = scene->ecs.getComponent<GeometryComponent>(entityPair.A);
			const GeometryComponent geometryComponentB = scene->ecs.getComponent<GeometryComponent>(entityPair.B);

			const Geometry3D& geometryA = scene->geometries[geometryComponentA.geometryIndex];
			const Geometry3D& geometryB = scene->geometries[geometryComponentB.geometryIndex];

			const Hull3D& hullA = scene->hulls[geometryA.hullIndex];
			const Hull3D& hullB = scene->hulls[geometryB.hullIndex];

			Hull3DType typeA = hullA.type;
			Hull3DType typeB = hullB.type;

			if (typeA > typeB) {
				typeA = hullB.type;
				typeB = hullA.type;
			}

			const Collider3D collider = COLLIDER_TABLE[static_cast<uint32>(typeA)][static_cast<uint32>(typeB)];

			if (!collider(rigidBodyA, rigidBodyB, hullA, hullB, &contactPair.manifold)) {
				// Either a degenerate case, or the entities are not overlapping.
				contactPairs.erase(entityPair);

				continue;
			}

			const std::map<const EntityPair, ContactPair3D>::iterator contactPairIterator = contactPairs.find(entityPair);

			if (contactPairIterator == contactPairs.end()) {
				contactPair.A = entityPair.A;
				contactPair.B = entityPair.B;

				contactPairs.insert({entityPair, contactPair});

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

			rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + G) * deltaTime;

			componentIndex += 1;
		}
	}

	void PhysicsSimulation3D::updateContactManifolds() {
		for (std::pair<const EntityPair, ContactPair3D>& pair : contactPairs) {
			ContactPair3D& contactPair = pair.second;

			const GeometryComponent geometryComponentA = scene->ecs.getComponent<GeometryComponent>(contactPair.A);
			const GeometryComponent geometryComponentB = scene->ecs.getComponent<GeometryComponent>(contactPair.B);

			const Geometry3D& geometryA = scene->geometries[geometryComponentA.geometryIndex];
			const Geometry3D& geometryB = scene->geometries[geometryComponentB.geometryIndex];

			const Hull3D& hullA = scene->hulls[geometryA.hullIndex];
			const Hull3D& hullB = scene->hulls[geometryB.hullIndex];

			RigidBody3DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody3DComponent>(contactPair.A);
			RigidBody3DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody3DComponent>(contactPair.B);

			if (dot(contactPair.manifold.normal, UP) > 0.0f) {
				rigidBodyB.groundEntity = contactPair.A;
			}

			vec3<float32> normalAntiParallel = RIGHT;

			if (abs(contactPair.manifold.normal[0]) > 0.9f) {
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

				const float32 penetrationBias = avg(hullA.getPenetrationBias(), hullB.getPenetrationBias());

				contact.bias = -penetrationBias * inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
					const vec3<float32> j =
						contactPair.manifold.normal * contact.accumulatedNormalImpulse +
						contactPair.manifold.tangent0 * contact.accumulatedTangent0Impulse +
						contactPair.manifold.tangent1 * contact.accumulatedTangent1Impulse;

					rigidBodyA.applyImpulse(-j, contact.rA);
					rigidBodyB.applyImpulse(j, contact.rB);
				#endif

				if (isnan(rigidBodyA.linearVelocity[0]) || isnan(rigidBodyB.linearVelocity[0])) BREAKPOINT();

				contactIndex += 1;
			}
		}
	}

	void PhysicsSimulation3D::applyImpulses() {
		uint32 iteration = 0;

		while (iteration < SEQUENTIAL_IMPULSE_ITERATION_COUNT) {
			for (std::pair<const EntityPair, ContactPair3D>& entityContactPair : contactPairs) {
				ContactPair3D& contactPair = entityContactPair.second;
				RigidBody3DComponent& rigidBodyA = scene->ecs.getComponent<RigidBody3DComponent>(contactPair.A);
				RigidBody3DComponent& rigidBodyB = scene->ecs.getComponent<RigidBody3DComponent>(contactPair.B);

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

			// TODO(debug): Apply a rotational constraint to the player entity.
			{
				scene->ecs.getComponent<RigidBody3DComponent>(1).angularVelocity = {0.0f, 0.0f, 0.0f};
			}

			iteration += 1;
		}
	}

	void PhysicsSimulation3D::integrate() {
		ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();
		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const GeometryComponent geometryComponent = scene->ecs.getComponent<GeometryComponent>(entity);
			const Geometry3D& geometry = scene->geometries[geometryComponent.geometryIndex];
			const Hull3D& hull = scene->hulls[geometry.hullIndex];

			rigidBodyComponent.position += rigidBodyComponent.linearVelocity * deltaTime;
			rigidBodyComponent.rotation += rigidBodyComponent.angularVelocity * deltaTime;
			rigidBodyComponent.updateOrientation();
			rigidBodyComponent.updateWorld(hull.centroid);
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