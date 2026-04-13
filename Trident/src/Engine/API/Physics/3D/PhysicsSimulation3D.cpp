// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/PhysicsSimulation3D.hpp"

namespace Trident {
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
		// TODO
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
		// TODO
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
			const Geometry3D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];
			const Hull3D& hull = scene->getHulls()[geometry.hullIndex];

			rigidBodyComponent.position += rigidBodyComponent.linearVelocity * deltaTime;
			rigidBodyComponent.rotation += rigidBodyComponent.angularVelocity * deltaTime;
			rigidBodyComponent.updateWorld(hull.centroid);

			rigidBodyComponent.force = {0.0_N, 0.0_N, 0.0_N};

			componentIndex += 1;
		}
	}
}