// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"

namespace Trident {
	void clearPairs(PhysicsSimulation2D& simulation) {
		simulation.entityPairs.clear();
	}

	const Hull2D& getHull(const PhysicsSimulation2D& simulation, Entity entity) {
		const view<Hull2D>& hulls = simulation.scene->getHulls();
		const Geometry2DComponent geometryComponent = simulation.scene->ecs.getComponent<Geometry2DComponent>(entity);
		const Geometry2D& geometry = simulation.scene->getGeometries()[geometryComponent.geometryIndex];

		ENGINE_ASSERT(geometry.hullIndex < hulls.count(), "The geometry doesn't have a hull.");

		return hulls[geometry.hullIndex];
	}
}