// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Axis.hpp"
#include "Engine/API/Geometry/Index.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	Scene3D::Scene3D(ref<Scene3DAsset> sceneAsset, uint64 alignment) :
		Scene({
			// TODO(scene/3d): Remove these constructor parameters.
			.dynamicVertexCount = 0,
			.dynamicIndexCount = 0
		}),
		asset(sceneAsset)
	{
		loadGeometries(alignment);
		loadEntityComponentMappings();
		updateRigidBodyFacePlanes();
	}

	void Scene3D::loadGeometries(uint64 alignment) {
		hulls = sequence<Hull3D>(asset->hullCount);
		hullEdges = sequence<Edge3D>(asset->hullEdgeCount);
		hullFaces = sequence<Plane3D>(asset->hullFaceCount);
		hullFaceEdgeIndices = sequence<uint16>(asset->hullFaceEdgeIndexCount);
		rigidBodyFacePlanes = sequence<Plane3D>(asset->rigidBodyFacePlaneCount);

		// TODO(scene/3d): Restore.
		/* uint32 geometryIndex = 0;
		uint32 hullIndex = 0;
		uint32 hullEdgeOffset = 0;
		uint32 hullFaceOffset = 0;
		uint16 hullFaceEdgeIndexOffset = 0;
		uint32 hullLineIndexOffset = 0;

		while (geometryIndex < geometries.count()) {
			const ref<Geometry3DAsset> geometryAsset = geometryAssets[geometryIndex];

			geometryTriangleTangentComponentRange.count = static_cast<uint32>(geometryAsset->tangents.count());

			geometries[geometryIndex] = Geometry3D({
				.triangleCount = geometryIndexRange.count / 3,
				.vertexPositionComponentRange = geometryVertexPositionComponentRange,
				.vertexNormalComponentRange = geometryVertexNormalComponentRange,
				.vertexTextureCoordinateComponentRange = geometryVertexTextureCoordinateComponentRange,
				.triangleTangentComponentOffset = geometryTriangleTangentComponentRange.offset,
				.indexOffset = geometryIndexRange.offset,
			});

			if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
				geometries[geometryIndex].hullIndex = hullIndex;

				std::copy(geometryAsset->getHull3DEdges().begin(), geometryAsset->getHull3DEdges().end(), &hullEdges[hullEdgeOffset]);
				std::copy(geometryAsset->getHull3DFaces().begin(), geometryAsset->getHull3DFaces().end(), &hullFaces[hullFaceOffset]);
				std::copy(geometryAsset->getHull3DFaceEdgeIndices().begin(), geometryAsset->getHull3DFaceEdgeIndices().end(), &hullFaceEdgeIndices[hullFaceEdgeIndexOffset]);
				std::copy(geometryAsset->getHull3DLineIndices().begin(), geometryAsset->getHull3DLineIndices().end(), pointer_cast<uint16*>(&dataBuffer[hullLineIndexByteOffset + hullLineIndexOffset * sizeof(uint16)]));

				const view<svec3<float32>> vertexPositions(pointer_cast<const svec3<float32>*>(&dataBuffer[geometryVertexPositionComponentRange.offset * sizeof(float32)]), geometryVertexPositionComponentRange.count / 3);
				const view<Edge3D> edges(&hullEdges[hullEdgeOffset], geometryAsset->getHull3DEdges().count());
				const view<Plane3D> faces(&hullFaces[hullFaceOffset], geometryAsset->getHull3DFaces().count());
				const view<uint16> faceEdgeIndices(&hullFaceEdgeIndices[hullFaceEdgeIndexOffset], geometryAsset->getHull3DFaceEdgeIndices().count());
				const range<uint32, uint16> lineIndexRange = {hullLineIndexOffset, static_cast<uint16>(geometryAsset->getHull3DLineIndices().count())};

				hulls[hullIndex] = {
					.centroid = geometryAsset->getHullCentroid(),
					.radius = geometryAsset->getHullRadius(),
					.vertexPositions = vertexPositions,
					.edges = edges,
					.faces = faces,
					.faceEdgeIndices = faceEdgeIndices,
					.lineIndexRange = lineIndexRange,
					.type = static_cast<Hull3DType>(geometryAsset->getHullType()),
				};

				hullIndex += 1;
				hullEdgeOffset += static_cast<uint32>(geometryAsset->getHull3DEdges().count());
				hullFaceOffset += static_cast<uint32>(geometryAsset->getHull3DFaces().count());
				hullFaceEdgeIndexOffset += static_cast<uint16>(geometryAsset->getHull3DFaceEdgeIndices().count());
				hullLineIndexOffset += lineIndexRange.count;
			}

			geometryTriangleTangentComponentRange.offset += geometryTriangleTangentComponentRange.count;

			geometryIndex += 1;
		} */
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

		// TODO(scene/3d): Restore.
		/* index rigidBodyFacePlaneOffset = 0;

		for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : asset->rigidBodyMappings) {
			ecs.setComponent<RigidBody3DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);

			RigidBody3DComponent& rigidBodyComponent = ecs.getComponent<RigidBody3DComponent>(rigidBodyMapping.entity);

			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(rigidBodyMapping.entity);
			const Geometry3D& geometry = geometries[geometryComponent.geometryIndex];

			if (geometry.hullIndex != max<uint32>()) {
				const Hull3D& hull = hulls[geometry.hullIndex];

				rigidBodyComponent.facePlanes = view<Plane3D>(&rigidBodyFacePlanes[rigidBodyFacePlaneOffset], hull.faces.count());

				rigidBodyFacePlaneOffset += rigidBodyComponent.facePlanes.count();
			}

			rigidBodyComponent.updateOrientation();
			rigidBodyComponent.updateWorld({0.0_m, 0.0_m, 0.0_m});
		} */
	}

	void Scene3D::updateRigidBodyFacePlanes() {
		ComponentPool<RigidBody3DComponent>& componentPool = ecs.getComponentPool<RigidBody3DComponent>();
		index componentIndex = 0;
		index rigidBodyFaceOffset = 0;

		while (componentIndex < componentPool.componentCount) {
			const Entity entity = componentPool.componentIndexToEntity[componentIndex];
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(entity);
			const Geometry3D& geometry = geometries[geometryComponent.geometryIndex];

			if (geometry.hullIndex == max<uint32>()) {
				componentIndex += 1;

				continue;
			}

			RigidBody3DComponent& rigidBodyComponent = componentPool.components[componentIndex];

			const mat4x4<float32> transposeInverseWorld = transpose(inverse(rigidBodyComponent.world));
			const Hull3D& hull = hulls[geometry.hullIndex];

			index faceIndex = 0;

			while (faceIndex < hull.faces.count()) {
				const Plane3D face = hull.faces[faceIndex];

				Plane3D& plane = rigidBodyFacePlanes[rigidBodyFaceOffset + faceIndex];

				// TODO(rigid-body/uniform-scaling): Remove non-uniform scaling properties on the rigid bodies.
				plane.normal = svec3<float32>(normalized(multiplyTransform(vec3<float32>(face.normal), transposeInverseWorld)));
				plane.distance = dot(multiplyTransform(vec3<float32>(face.normal) * face.distance, rigidBodyComponent.world), vec3<float32>(plane.normal));

				faceIndex += 1;
			}

			rigidBodyFaceOffset += hull.faces.count();
			componentIndex += 1;
		}
	}
}