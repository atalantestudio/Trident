// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Build/Cluster.hpp"
#include "Engine/Build/ClusteredGeometry.hpp"
#include "Engine/Build/ClusteredInstance.hpp"
#include "Engine/Build/ClusteredScene.hpp"

namespace Trident {
	static constexpr uint32 CLUSTER_TRIANGLE_COUNT = 128;
	static constexpr uint32 CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;

	static ClusteredScene clusterizeScene(const Scene3D& scene) {
		ClusteredScene clusteredScene{};
		clusteredScene.geometries = sequence<ClusteredGeometry>(scene.getGeometryCount());
		clusteredScene.materials = sequence<Material>(scene.getMaterialCount());
		clusteredScene.materialReferences = sequence<std::vector<MaterialReference>>(scene.getMaterialCount());

		std::copy_n(scene.getMaterials().begin(), clusteredScene.materials.count(), &clusteredScene.materials[0]);
		std::copy_n(scene.getAllMaterialReferences().begin(), clusteredScene.materialReferences.count(), &clusteredScene.materialReferences[0]);

		const view<ref<Geometry3D>> geometries = scene.getGeometries();
		const view<RigidBody3D> instances = scene.getInstances();

		uint64 totalVertexPositionComponentCount = 0;
		uint64 totalVertexNormalComponentCount = 0;
		uint64 totalVertexTextureCoordinateComponentCount = 0;
		uint32 totalIndexCount = 0;
		uint32 totalClusterCount = 0;
		uint32 totalInstanceCount = 0;

		uint32 clusterOffset = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < scene.getGeometryCount()) {
			const ref<Geometry3D> geometry = geometries[geometryIndex];
			const uint64 indexCount = geometry->getVertexPositionIndices().count();

			// TODO: Position indices SHOULD BE AS MANY as normal indices (and texture coordinate indices?).
			DEBUG_ASSERT(indexCount == geometry->getVertexNormalIndices().count() && indexCount == geometry->getVertexTextureCoordinateIndices().count());

			const uint32 clusterCount = static_cast<uint32>(ceil(static_cast<float32>(indexCount) / CLUSTER_INDEX_COUNT));
			const uint32 instanceCount = scene.getInstanceCount(geometryIndex);

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.vertexCount = static_cast<uint32>(geometry->getVertexPositionComponents().count() / 3);
			clusteredGeometry.clusterCount = clusterCount;
			clusteredGeometry.clusterOffset = clusterOffset;
			clusteredGeometry.instanceCount = instanceCount;

			totalVertexPositionComponentCount += geometry->getVertexPositionComponents().count();
			totalVertexNormalComponentCount += geometry->getVertexNormalComponents().count();
			totalVertexTextureCoordinateComponentCount += geometry->getVertexTextureCoordinateComponents().count();
			totalIndexCount += clusterCount * CLUSTER_INDEX_COUNT;
			totalClusterCount += clusterCount * instanceCount;
			totalInstanceCount += instanceCount;

			clusterOffset += clusterCount;
			geometryIndex += 1;
		}

		clusteredScene.clusters = sequence<Cluster>(totalClusterCount);
		clusteredScene.instances = sequence<ClusteredInstance>(totalInstanceCount);

		clusteredScene.vertexPositionComponents = sequence<float32>(totalVertexPositionComponentCount);
		clusteredScene.vertexNormalComponents = sequence<float32>(totalVertexNormalComponentCount);
		clusteredScene.vertexTextureCoordinateComponents = sequence<float32>(totalVertexTextureCoordinateComponentCount);
		clusteredScene.vertexPositionIndices = sequence<uint32>(totalIndexCount);
		clusteredScene.vertexNormalIndices = sequence<uint32>(totalIndexCount);
		clusteredScene.vertexTextureCoordinateIndices = sequence<uint32>(totalIndexCount);

		uint32 vertexPositionComponentOffset = 0;
		uint32 vertexNormalComponentOffset = 0;
		uint32 vertexTextureCoordinateComponentOffset = 0;
		uint32 vertexOffset = 0;

		// Index offset relative to the parent cluster. Includes degenerate indices.
		uint32 indexOffset = 0;

		geometryIndex = 0;

		while (geometryIndex < scene.getGeometryCount()) {
			const ref<Geometry3D> geometry = geometries[geometryIndex];

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.vertexOffset = vertexOffset;

			std::copy(geometry->getVertexPositionComponents().begin(), geometry->getVertexPositionComponents().end(), &clusteredScene.vertexPositionComponents[vertexPositionComponentOffset]);
			std::copy(geometry->getVertexNormalComponents().begin(), geometry->getVertexNormalComponents().end(), &clusteredScene.vertexNormalComponents[vertexNormalComponentOffset]);
			std::copy(geometry->getVertexTextureCoordinateComponents().begin(), geometry->getVertexTextureCoordinateComponents().end(), &clusteredScene.vertexTextureCoordinateComponents[vertexTextureCoordinateComponentOffset]);

			std::copy(geometry->getVertexPositionIndices().begin(), geometry->getVertexPositionIndices().end(), &clusteredScene.vertexPositionIndices[indexOffset]);
			std::copy(geometry->getVertexNormalIndices().begin(), geometry->getVertexNormalIndices().end(), &clusteredScene.vertexNormalIndices[indexOffset]);
			std::copy(geometry->getVertexTextureCoordinateIndices().begin(), geometry->getVertexTextureCoordinateIndices().end(), &clusteredScene.vertexTextureCoordinateIndices[indexOffset]);

			vertexOffset += clusteredGeometry.vertexCount;
			indexOffset += clusteredGeometry.clusterCount * CLUSTER_INDEX_COUNT;
			vertexPositionComponentOffset += static_cast<uint32>(geometry->getVertexPositionComponents().count());
			vertexNormalComponentOffset += static_cast<uint32>(geometry->getVertexNormalComponents().count());
			vertexTextureCoordinateComponentOffset += static_cast<uint32>(geometry->getVertexTextureCoordinateComponents().count());

			geometryIndex += 1;
		}

		uint32 instanceIndex = 0;
		uint32 clusterIndex;

		clusterOffset = 0;

		while (instanceIndex < instances.count()) {
			const RigidBody3D& instance = instances[instanceIndex];
			const ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[instance.geometryIndex];

			ClusteredInstance& clusteredInstance = clusteredScene.instances[instanceIndex];
			clusteredInstance.transform = instance.transform;
			clusteredInstance.geometryIndex = instance.geometryIndex;

			// TODO: Pack material index + material reference index?
			clusteredInstance.materialIndex = instance.materialReferenceIndex;

			clusteredInstance.clusterOffset = clusteredGeometry.clusterOffset - clusterOffset;

			clusterIndex = 0;

			while (clusterIndex < clusteredGeometry.clusterCount) {
				Cluster& cluster = clusteredScene.clusters[clusterOffset + clusterIndex];
				cluster.instanceIndex = instanceIndex;

				clusterIndex += 1;
			}

			clusterOffset += clusteredGeometry.clusterCount;
			instanceIndex += 1;
		}

		return clusteredScene;
	}
}