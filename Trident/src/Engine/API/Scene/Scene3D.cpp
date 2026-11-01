// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Axis.hpp"
#include "Engine/API/Geometry/Index.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/Geometry3DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	Scene3D::Scene3D(ref<Scene3DAsset> sceneAsset) :
		Scene({
			// TODO(scene/3d): Remove these constructor parameters.
			.dynamicVertexCount = 0,
			.dynamicIndexCount = 0
		}),
		asset(sceneAsset)
	{
		loadEntityComponentMappings();
	}

	void Scene3D::loadEntityComponentMappings() {
		ecs.registerComponentPool<Geometry3DComponent>();

		// TODO(ecs/scene-components): Use Transform3DComponent for entities that are fixed in place?
		ecs.registerComponentPool<RigidBody3DComponent>();

		Entity entity = 0;

		while (entity < asset->entityCount) {
			ecs.createEntity();

			entity += 1;
		}

		for (const EntityComponentMapping<Geometry3DComponent>& geometryMapping : asset->geometryMappings) {
			ecs.setComponent<Geometry3DComponent>(geometryMapping.entity, geometryMapping.component);
		}

		index rigidBodyFacePlaneOffset = 0;

		for (EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : asset->rigidBodyMappings) {
			rigidBodyMapping.component.updateOrientation();
			rigidBodyMapping.component.updateWorld({0.0f, 0.0f, 0.0f});

			ecs.setComponent<RigidBody3DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);
		}
	}
}