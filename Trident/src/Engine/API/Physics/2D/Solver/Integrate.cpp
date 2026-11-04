// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	void integrate(PhysicsSimulation2D& simulation) {
		ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = simulation.scene->ecs.getComponentPool<RigidBody2DComponent>();
		uint32 componentIndex = 0;

		// TODO(physics/2d/integration): Integration for static entities (e.g. a moving platform that isn't affected by other forces)?
		// TODO(physics/2d/integration): Integration for dynamic entities with infinite mass?
		// TODO(physics/2d/integration): Integration for dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			// TODO(physics/2d/integration): Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			const Hull2D& hull = getHull(simulation, rigidBodyComponentPool.componentIndexToEntity[componentIndex]);

			rigidBodyComponent.position += rigidBodyComponent.linearVelocity * simulation.deltaTime;
			rigidBodyComponent.rotation += rigidBodyComponent.angularVelocity * simulation.deltaTime;
			rigidBodyComponent.updateWorld(hull.centroid);

			// TODO(physics/2d/integration)
			// rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + vec2<float32>(0.0f, G)) * halfDeltaTime;
			// rigidBodyComponent.angularVelocity += rigidBodyComponent.inverseInertia * halfDeltaTime;

			rigidBodyComponent.force = {0.0_N, 0.0_N};

			componentIndex += 1;
		}
	}
}