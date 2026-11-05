// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"

namespace Trident {
	void clearPairs(PhysicsSimulation2D& simulation) {
		simulation.collisionPairs.clear();
	}

	const Hull2D& getHull(const PhysicsSimulation2D& simulation, Entity entity) {
		const view<Hull2D>& hulls = simulation.scene->getHulls();
		const Geometry2DComponent geometryComponent = simulation.scene->ecs.getComponent<Geometry2DComponent>(entity);
		const Geometry2D& geometry = simulation.scene->getGeometries()[geometryComponent.geometryIndex];

		ENGINE_ASSERT(geometry.hullIndex < hulls.count(), "The geometry doesn't have a hull.");

		return hulls[geometry.hullIndex];
	}

	void applyForce(PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId, vec2<float32> F) {
		RigidBody2D& rigidBody = simulation.rigidBodies[rigidBodyId];
		rigidBody.force += F;
	}

	void applyImpulse(RigidBody2D& rigidBody, vec2<float32> j, vec2<float32> r) {
		rigidBody.state.linearVelocity += j * rigidBody.inverseMass;

		// Subtract to get a clockwise rotation.
		rigidBody.state.angularVelocity -= parallelogramArea(r, j) * rigidBody.inverseInertia;
	}

	mat3x3<float32> computeWorld(const PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId) {
		const RigidBody2D& rigidBody = simulation.rigidBodies[rigidBodyId];

		return {
			translation<mat3x3<float32>>(-rigidBody.worldCentroid) *
			rotationAngle<mat3x3<float32>>(rigidBody.rotation) *
			translation<mat3x3<float32>>(rigidBody.position)
		};
	}
}