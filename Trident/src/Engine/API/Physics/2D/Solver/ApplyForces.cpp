// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	void applyForces(PhysicsSimulation2D& simulation) {
		// TODO(physics/2d/apply-forces): Implement torque.
		constexpr float32 torque = 0.0_Nm;

		ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = simulation.scene->ecs.getComponentPool<RigidBody2DComponent>();
		uint32 componentIndex = 0;

		// TODO(physics/2d/apply-forces): Integrate velocities on dynamic resting entities (e.g. an object on a moving platform may move due to inertia).
		while (componentIndex < rigidBodyComponentPool.componentCount) {
			RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			// TODO(physics/2d/apply-forces): Exclude dynamic entities with an inverse mass of 0 from the loop.
			if (rigidBodyComponent.inverseMass == 0.0_kg) {
				componentIndex += 1;

				continue;
			}

			// TODO(physics/2d/apply-forces)
			// Integrate gravity like in https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html.
			// This looks like Velocity Verlet for gravity to me?
			// rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + simulation.gravity) * simulation.halfDeltaTime;
			rigidBodyComponent.linearVelocity += (rigidBodyComponent.force * rigidBodyComponent.inverseMass + simulation.gravity) * simulation.deltaTime;
			// rigidBodyComponent.angularVelocity += rigidBodyComponent.inverseInertia * halfDeltaTime;
			rigidBodyComponent.angularVelocity += torque * rigidBodyComponent.inverseInertia * simulation.deltaTime;

			componentIndex += 1;
		}
	}
}