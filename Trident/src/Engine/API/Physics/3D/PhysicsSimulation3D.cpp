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

			// TODO(physics/3d/merge-contact-manifolds)
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
		uint8 contactPointIndex;

		// Calculate combined values in each pair (static/dynamic friction, restitution, etc.).
		// For each contact, calculate mass normal, mass tangent and bias value.
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

			// TODO(physics/3d/tangents)
			// contactPair.manifold.tangent = perpendicularRight(contactPair.manifold.normal);

			// TODO(physics/3d/friction)
			// contactPair.combinedFriction = sqrt(rigidBodyA.friction * rigidBodyB.friction);

			contactPointIndex = 0;

			while (contactPointIndex < contactPair.manifold.contactCount) {
				Contact3D& contact = contactPair.manifold.contacts[contactPointIndex];

				// Distance from the contact to the entity's COM.
				contact.rA = contact.position - rigidBodyA.position;
				contact.rB = contact.position - rigidBodyB.position;

				const float32 inverseMassSum = rigidBodyA.inverseMass + rigidBodyB.inverseMass;

				// Precompute normal mass, tangent mass and bias.
				const vec3<float32> rnA = cross(contact.rA, contactPair.manifold.normal);
				const vec3<float32> rnB = cross(contact.rB, contactPair.manifold.normal);
				const float32 massNormal =
					dot(rnA, rnA * rigidBodyA.inverseInertia) +
					dot(rnB, rnA * rigidBodyB.inverseInertia) +
					inverseMassSum;

				contact.massNormal = 1.0f / massNormal;

				const vec3<float32> rtA = cross(contact.rA, contactPair.manifold.tangent);
				const vec3<float32> rtB = cross(contact.rB, contactPair.manifold.tangent);
				const float32 massTangent =
					dot(rtA, rtA * rigidBodyA.inverseInertia) +
					dot(rtB, rtB * rigidBodyB.inverseInertia) +
					inverseMassSum;

				contact.massTangent = 1.0f / massTangent;

				const float32 penetrationBias = avg(hullA.getPenetrationBias(), hullB.getPenetrationBias());

				contact.bias = -penetrationBias * inverseDeltaTime * min(0.0f, contact.separation + ALLOWED_PENETRATION);

				#if defined(ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION)
					const vec3<float32> impulse = contactPair.manifold.normal * contact.accumulatedNormalImpulse + contactPair.manifold.tangent * contact.accumulatedTangentImpulse;

					rigidBodyA.applyImpulse(-impulse, contact.rA);
					rigidBodyB.applyImpulse(impulse, contact.rB);
				#endif

				contactPointIndex += 1;
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

				// TODO(debug)
				rigidBodyA.linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};
				rigidBodyB.linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};
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
			rigidBodyComponent.updateWorld(hull.centroid);

			rigidBodyComponent.force = {0.0_N, 0.0_N, 0.0_N};

			componentIndex += 1;
		}
	}
}