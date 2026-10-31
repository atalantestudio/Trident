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
		loadGeometries();
		loadEntityComponentMappings();
		updateRigidBodyFacePlanes();
	}

	void Scene3D::loadGeometries() {
		rigidBodyFacePlanes = sequence<Plane3D>(asset->rigidBodyFacePlaneCount);
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

		for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : asset->rigidBodyMappings) {
			ecs.setComponent<RigidBody3DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);

			RigidBody3DComponent& rigidBodyComponent = ecs.getComponent<RigidBody3DComponent>(rigidBodyMapping.entity);

			const Geometry3DComponent& geometryComponent = ecs.getComponent<Geometry3DComponent>(rigidBodyMapping.entity);

			if (geometryComponent.hasHull) {
				rigidBodyComponent.facePlanes = view<Plane3D>(&rigidBodyFacePlanes[rigidBodyFacePlaneOffset], geometryComponent.hull.faces.count());
				rigidBodyFacePlaneOffset += rigidBodyComponent.facePlanes.count();
			}

			rigidBodyComponent.updateOrientation();
			rigidBodyComponent.updateWorld({0.0_m, 0.0_m, 0.0_m});
		}
	}

	void Scene3D::updateRigidBodyFacePlanes() {
		ComponentPool<RigidBody3DComponent>& componentPool = ecs.getComponentPool<RigidBody3DComponent>();
		index componentIndex = 0;
		index rigidBodyFaceOffset = 0;

		while (componentIndex < componentPool.componentCount) {
			const Entity entity = componentPool.componentIndexToEntity[componentIndex];
			const Geometry3DComponent& geometryComponent = ecs.getComponent<Geometry3DComponent>(entity);

			if (!geometryComponent.hasHull) {
				componentIndex += 1;

				continue;
			}

			const Hull3D& hull = geometryComponent.hull;
			const RigidBody3DComponent& component = componentPool.components[componentIndex];

			index faceIndex = 0;

			while (faceIndex < hull.faces.count()) {
				const Plane3D face = hull.faces[faceIndex];

				Plane3D& plane = rigidBodyFacePlanes[rigidBodyFaceOffset + faceIndex];
				plane.normal = normalized(face.normal * inverse(component.orientation));
				plane.distance = dot(face.normal * face.distance * component.world, plane.normal);
				plane.edgeIndexRange = face.edgeIndexRange;

				faceIndex += 1;
			}

			rigidBodyFaceOffset += hull.faces.count();
			componentIndex += 1;
		}
	}
}