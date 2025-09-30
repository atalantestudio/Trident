#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace ProjectA {
	static constexpr uint32 CLUSTER_TRIANGLE_COUNT = 128;
	static constexpr uint32 CLUSTER_VERTEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;

	struct ClusteredInstance {
		uint32 geometryIndex;
	};

	struct Cluster {
		uint32 instanceIndex;
	};

	struct ClusteredGeometry {
		uint32 clusterCount;

		// Number of instances using this geometry.
		uint32 instanceCount;
	};

	struct ClusteredScene {
		sequence<Cluster> clusters;
		sequence<ClusteredGeometry> geometries;
		sequence<ClusteredInstance> instances;

		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexNormalComponents;
		sequence<float32> vertexTextureCoordinateComponents;

		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
	};

	static ClusteredScene clusterizeScene(const Scene3D& scene) {
		const ComponentPool<ref<Geometry3D>>& geometryPool = scene.getComponentPool<ref<Geometry3D>>();

		ClusteredScene clusteredScene{};
		clusteredScene.geometries = sequence<ClusteredGeometry>(geometryPool.componentCount);

		uint32 totalVertexPositionComponentCount = 0;
		uint32 totalVertexNormalComponentCount = 0;
		uint32 totalVertexTextureCoordinateComponentCount = 0;
		uint32 totalIndexCount = 0;
		uint32 totalClusterCount = 0;
		uint32 totalInstanceCount = 0;

		for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
			const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

			// NOTE: Vertex position indices SHOULD BE AS MANY as vertex normal indices.
			const uint32 indexCount = static_cast<uint32>(geometry->getVertexPositionIndices().count);
			const uint32 clusterCount = static_cast<uint32>(ceil(static_cast<float32>(indexCount) / CLUSTER_VERTEX_COUNT));

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.clusterCount = clusterCount;
			clusteredGeometry.instanceCount = scene.getInstanceCount(geometryIndex);

			totalVertexPositionComponentCount += static_cast<uint32>(geometry->getVertexPositionComponents().count);
			totalVertexNormalComponentCount += static_cast<uint32>(geometry->getVertexNormalComponents().count);
			totalVertexTextureCoordinateComponentCount += static_cast<uint32>(geometry->getVertexTextureCoordinateComponents().count);
			totalIndexCount += clusterCount * CLUSTER_VERTEX_COUNT;
			totalClusterCount += clusterCount;
			totalInstanceCount += clusteredGeometry.instanceCount;
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

		// Cluster-wise index offset.
		uint32 indexOffset = 0;
		uint32 instanceIndexOffset = 0;

		uint32 clusterOffset = 0;
		uint32 instanceOffset = 0;

		for (uint32 geometryIndex = 0; geometryIndex < geometryPool.componentCount; geometryIndex += 1) {
			const ref<Geometry3D>& geometry = geometryPool.components[geometryIndex];

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];

			std::copy(geometry->getVertexPositionComponents().begin(), geometry->getVertexPositionComponents().end(), clusteredScene.vertexPositionComponents.begin() + vertexPositionComponentOffset);
			std::copy(geometry->getVertexNormalComponents().begin(), geometry->getVertexNormalComponents().end(), clusteredScene.vertexNormalComponents.begin() + vertexNormalComponentOffset);
			std::copy(geometry->getVertexTextureCoordinateComponents().begin(), geometry->getVertexTextureCoordinateComponents().end(), clusteredScene.vertexTextureCoordinateComponents.begin() + vertexTextureCoordinateComponentOffset);

			for (uint32 relativeInstanceIndex = 0; relativeInstanceIndex < clusteredGeometry.instanceCount; relativeInstanceIndex += 1, instanceIndexOffset += 1) {
				// NOTE: Can the index buffers not be copied for each instance?
				std::copy(geometry->getVertexPositionIndices().begin(), geometry->getVertexPositionIndices().end(), clusteredScene.vertexPositionIndices.begin() + indexOffset);
				std::copy(geometry->getVertexNormalIndices().begin(), geometry->getVertexNormalIndices().end(), clusteredScene.vertexNormalIndices.begin() + indexOffset);
				std::copy(geometry->getVertexTextureCoordinateIndices().begin(), geometry->getVertexTextureCoordinateIndices().end(), clusteredScene.vertexTextureCoordinateIndices.begin() + indexOffset);

				indexOffset += clusteredGeometry.clusterCount * CLUSTER_VERTEX_COUNT;

				for (uint32 relativeClusterIndex = 0; relativeClusterIndex < clusteredGeometry.clusterCount; relativeClusterIndex += 1) {
					Cluster& cluster = clusteredScene.clusters[clusterOffset + relativeClusterIndex];
					cluster.instanceIndex = instanceIndexOffset;
				}

				clusterOffset += clusteredGeometry.clusterCount;

				ClusteredInstance& clusteredInstance = clusteredScene.instances[instanceOffset + relativeInstanceIndex];
				clusteredInstance.geometryIndex = geometryIndex;
			}

			vertexPositionComponentOffset += static_cast<uint32>(geometry->getVertexPositionComponents().count);
			vertexNormalComponentOffset += static_cast<uint32>(geometry->getVertexNormalComponents().count);
			vertexTextureCoordinateComponentOffset += static_cast<uint32>(geometry->getVertexTextureCoordinateComponents().count);
		}

		return clusteredScene;
	}
}