// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Axis.hpp"
#include "Engine/API/Geometry/Index.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
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
		hullEdges = sequence<Edge3D>(asset->hullEdgeCount);
		hullFaces = sequence<Plane3D>(asset->hullFaceCount);
		hullFaceEdgeIndices = sequence<uint16>(asset->hullFaceEdgeIndexCount);
		rigidBodyFacePlanes = sequence<Plane3D>(asset->rigidBodyFacePlaneCount);
	}

	void Scene3D::loadEntityComponentMappings() {
		ecs.registerComponentPool<GeometryComponent>();

		// TODO(ecs/scene-components): Use Transform3DComponent for entities that are fixed in place?
		ecs.registerComponentPool<RigidBody3DComponent>();

		Entity entity = 0;

		while (entity < asset->entityCount) {
			ecs.createEntity();

			entity += 1;
		}

		for (const EntityComponentMapping<GeometryComponent> geometryMapping : asset->geometryMappings) {
			ecs.setComponent<GeometryComponent>(geometryMapping.entity, geometryMapping.component);
		}

		index rigidBodyFacePlaneOffset = 0;

		for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : asset->rigidBodyMappings) {
			ecs.setComponent<RigidBody3DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);

			RigidBody3DComponent& rigidBodyComponent = ecs.getComponent<RigidBody3DComponent>(rigidBodyMapping.entity);

			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(rigidBodyMapping.entity);
			const uint32 hullIndex = asset->geometryIndexToHullIndex[geometryComponent.geometryIndex];

			if (hullIndex != max<uint32>()) {
				const Hull3D& hull = asset->hulls[hullIndex];

				rigidBodyComponent.facePlanes = view<Plane3D>(&rigidBodyFacePlanes[rigidBodyFacePlaneOffset], hull.faces.count());

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
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(entity);
			const uint32 hullIndex = asset->geometryIndexToHullIndex[geometryComponent.geometryIndex];

			if (hullIndex == max<uint32>()) {
				componentIndex += 1;

				continue;
			}

			const Hull3D& hull = asset->hulls[hullIndex];
			const mat4x4<float32>& world = componentPool.components[componentIndex].world;
			const mat4x4<float32> transposeInverseWorld = transpose(inverse(world));

			index faceIndex = 0;

			while (faceIndex < hull.faces.count()) {
				const Plane3D face = hull.faces[faceIndex];

				Plane3D& plane = rigidBodyFacePlanes[rigidBodyFaceOffset + faceIndex];

				// TODO(rigid-body/uniform-scaling): Remove non-uniform scaling properties on the rigid bodies.
				plane.normal = normalized(face.normal * transposeInverseWorld);
				plane.distance = dot(face.normal * face.distance * world, plane.normal);

				faceIndex += 1;
			}

			rigidBodyFaceOffset += hull.faces.count();
			componentIndex += 1;
		}
	}
}