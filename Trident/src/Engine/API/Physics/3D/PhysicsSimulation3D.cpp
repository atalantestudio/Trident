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
		// TODO
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