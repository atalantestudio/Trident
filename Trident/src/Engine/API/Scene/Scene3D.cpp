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
		asset(sceneAsset),
		vertexPositionComponentRange(0, 0),
		vertexNormalComponentRange(0, 0),
		vertexTextureCoordinateComponentRange(0, 0),
		triangleTangentComponentRange(0, 0),
		indexRange(0, 0)
	{
		loadGeometries(alignment);
		loadEntityComponentMappings();
		updateRigidBodyFacePlanes();
	}

	void Scene3D::loadGeometries(uint64 alignment) {
		// TODO(scene/3d): Restore.
		/* const sequence<ref<GeometryAsset>>& geometryAssets = asset->geometryAssets;
		const uint64 vertexComponentCount = (
			asset->dynamicVertexPositionCount * 3 +
			asset->dynamicVertexNormalCount * 3 +
			asset->dynamicVertexTextureCoordinateCount * 2
		);

		indexCount = static_cast<uint32>(asset->dynamicIndexCount);

		vertexPositionComponentRange.count = ceil(asset->dynamicVertexPositionCount * sizeof(svec3<float32>), alignment);
		vertexNormalComponentRange.count = ceil(asset->dynamicVertexNormalCount * sizeof(svec3<float32>), alignment);
		vertexTextureCoordinateComponentRange.count = ceil(asset->dynamicVertexTextureCoordinateCount * sizeof(svec2<float32>), alignment);
		triangleTangentComponentRange.count = ceil(asset->dynamicTriangleTangentCount * sizeof(svec3<float32>), alignment);
		indexRange.count = ceil(asset->dynamicIndexCount * sizeof(GeometryIndex3D) + asset->hullLineIndexCount * sizeof(uint16), alignment);

		range<uint64, uint64>* ranges = &vertexPositionComponentRange;
		uint64 dataBufferSize = 0;

		// Propagate the range offset.
		while (ranges < &indexRange) {
			ranges[1].offset = ranges[0].offset + ranges[0].count;
			dataBufferSize += ranges[0].count;

			ranges += 1;
		}

		dataBufferSize += ranges->count;

		dataBuffer = sequence<uint8>(dataBufferSize);
		geometries = sequence<Geometry3D>(geometryAssets.count());
		hulls = sequence<Hull3D>(asset->hullCount);
		hullEdges = sequence<Edge3D>(asset->hullEdgeCount);
		hullFaces = sequence<Plane3D>(asset->hullFaceCount);
		hullFaceEdgeIndices = sequence<uint16>(asset->hullFaceEdgeIndexCount);
		rigidBodyFacePlanes = sequence<Plane3D>(asset->rigidBodyFacePlaneCount);

		uint64 byteOffset = vertexPositionComponentRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<float32>& vertexPositionComponents = geometryAsset->getVertexPositionComponents();

			std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), pointer_cast<float32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexPositionComponents.count() * sizeof(float32);
		}

		byteOffset = vertexNormalComponentRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<float32>& vertexNormalComponents = geometryAsset->getVertexNormalComponents();

			std::copy(vertexNormalComponents.begin(), vertexNormalComponents.end(), pointer_cast<float32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexNormalComponents.count() * sizeof(float32);
		}

		byteOffset = vertexTextureCoordinateComponentRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<float32>& vertexTextureCoordinateComponents = geometryAsset->getVertexTextureCoordinateComponents();

			std::copy(vertexTextureCoordinateComponents.begin(), vertexTextureCoordinateComponents.end(), pointer_cast<float32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexTextureCoordinateComponents.count() * sizeof(float32);
		}

		byteOffset = triangleTangentComponentRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<float32>& triangleTangentComponents = geometryAsset->getTriangleTangentComponents();

			std::copy(triangleTangentComponents.begin(), triangleTangentComponents.end(), pointer_cast<float32*>(&dataBuffer[byteOffset]));

			byteOffset += triangleTangentComponents.count() * sizeof(float32);
		}

		byteOffset = indexRange.offset;

		GeometryIndex3D* indices = pointer_cast<GeometryIndex3D*>(&dataBuffer[byteOffset]);
		uint32 indexOffset = 0;
		uint32 vertexIndexOffset = 0;
		uint32 normalIndexOffset = 0;
		uint32 textureCoordinateIndexOffset = 0;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const view<uint32> vertexPositionIndices = geometryAsset->getVertexPositionIndices();
			const view<uint32> vertexNormalIndices = geometryAsset->getVertexNormalIndices();
			const view<uint32> vertexTextureCoordinateIndices = geometryAsset->getVertexTextureCoordinateIndices();

			uint32 indexIndex = 0;

			while (indexIndex < geometryAsset->getIndexCount()) {
				GeometryIndex3D& index = indices[indexOffset + indexIndex];
				index.positionIndex = vertexIndexOffset + vertexPositionIndices[indexIndex];
				index.normalIndex = normalIndexOffset + vertexNormalIndices[indexIndex];
				index.textureCoordinateIndex = textureCoordinateIndexOffset + vertexTextureCoordinateIndices[indexIndex];

				indexIndex  += 1;
			}

			indexOffset += indexIndex;
			vertexIndexOffset += static_cast<uint32>(geometryAsset->getVertexPositionComponents().count() / 3);
			normalIndexOffset += static_cast<uint32>(geometryAsset->getVertexNormalComponents().count() / 3);
			textureCoordinateIndexOffset += static_cast<uint32>(geometryAsset->getVertexTextureCoordinateComponents().count() / 2);
			byteOffset += indexIndex * sizeof(GeometryIndex3D);
		}

		hullLineIndexByteOffset = byteOffset;

		// NOTE: The offsets in these ranges are relative to the start of the corresponding descriptor buffer.
		range<uint32, uint32> geometryVertexPositionComponentRange = {0, 0};
		range<uint32, uint32> geometryVertexNormalComponentRange = {0, 0};
		range<uint32, uint32> geometryVertexTextureCoordinateComponentRange = {0, 0};
		range<uint32, uint32> geometryTriangleTangentComponentRange = {0, 0};
		range<uint32, uint32> geometryIndexRange = {0, 0};

		uint32 geometryIndex = 0;
		uint32 hullIndex = 0;
		uint32 hullEdgeOffset = 0;
		uint32 hullFaceOffset = 0;
		uint16 hullFaceEdgeIndexOffset = 0;
		uint32 hullLineIndexOffset = 0;

		while (geometryIndex < geometries.count()) {
			const ref<GeometryAsset> geometryAsset = geometryAssets[geometryIndex];

			geometryVertexPositionComponentRange.count = static_cast<uint32>(geometryAsset->getVertexPositionComponents().count());
			geometryVertexNormalComponentRange.count = static_cast<uint32>(geometryAsset->getVertexNormalComponents().count());
			geometryVertexTextureCoordinateComponentRange.count = static_cast<uint32>(geometryAsset->getVertexTextureCoordinateComponents().count());
			geometryTriangleTangentComponentRange.count = static_cast<uint32>(geometryAsset->getTriangleTangentComponents().count());
			geometryIndexRange.count = static_cast<uint32>(geometryAsset->getVertexPositionIndices().count());

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

			geometryVertexPositionComponentRange.offset += geometryVertexPositionComponentRange.count;
			geometryVertexNormalComponentRange.offset += geometryVertexNormalComponentRange.count;
			geometryVertexTextureCoordinateComponentRange.offset += geometryVertexTextureCoordinateComponentRange.count;
			geometryTriangleTangentComponentRange.offset += geometryTriangleTangentComponentRange.count;
			geometryIndexRange.offset += geometryIndexRange.count;

			geometryIndex += 1;
		} */
	}

	void Scene3D::loadEntityComponentMappings() {
		// TODO(scene/3d): Restore.
		/* ecs.registerComponentPool<GeometryComponent>();

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
		// TODO(scene/3d): Restore.
		/* ComponentPool<RigidBody3DComponent>& componentPool = ecs.getComponentPool<RigidBody3DComponent>();
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
		} */
	}
}