#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace ProjectA {
	static constexpr uint32 CLUSTER_TRIANGLE_COUNT = 128;
	static constexpr uint32 CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;

	struct ClusteredInstance {
		mat4x4<float32> transform;
		uint32 geometryIndex;
		sint32 clusterOffset;
	};

	struct Cluster {
		uint32 instanceIndex;
	};

	struct ClusteredGeometry {
		uint32 vertexCount;

		// Per instance cluster count.
		uint32 clusterCount;

		// Index of the first cluster belonging to this geometry.
		uint32 clusterOffset;

		// Number of instances using this geometry.
		uint32 instanceCount;

		// Offset to be added to the index before indexing into the vertex buffer.
		uint32 vertexOffset;
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
		const sequence<ref<Geometry3D>>& geometries = scene.getGeometries();

		ClusteredScene clusteredScene{};
		clusteredScene.geometries = sequence<ClusteredGeometry>(scene.getGeometryCount());

		uint64 totalVertexPositionComponentCount = 0;
		uint64 totalVertexNormalComponentCount = 0;
		uint64 totalVertexTextureCoordinateComponentCount = 0;
		uint32 totalIndexCount = 0;
		uint32 totalClusterCount = 0;
		uint32 totalInstanceCount = 0;

		uint32 clusterOffset = 0;

		for (uint32 geometryIndex = 0; geometryIndex < scene.getGeometryCount(); geometryIndex += 1) {
			const ref<Geometry3D>& geometry = geometries[geometryIndex];

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

		// Cluster-wise index offset, including degenerate indices.
		uint32 indexOffset = 0;

		for (uint32 geometryIndex = 0; geometryIndex < scene.getGeometryCount(); geometryIndex += 1) {
			const ref<Geometry3D>& geometry = geometries[geometryIndex];

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.vertexOffset = vertexOffset;

			std::copy(geometry->getVertexPositionComponents().begin(), geometry->getVertexPositionComponents().end(), clusteredScene.vertexPositionComponents.begin() + vertexPositionComponentOffset);
			std::copy(geometry->getVertexNormalComponents().begin(), geometry->getVertexNormalComponents().end(), clusteredScene.vertexNormalComponents.begin() + vertexNormalComponentOffset);
			std::copy(geometry->getVertexTextureCoordinateComponents().begin(), geometry->getVertexTextureCoordinateComponents().end(), clusteredScene.vertexTextureCoordinateComponents.begin() + vertexTextureCoordinateComponentOffset);

			std::copy(geometry->getVertexPositionIndices().begin(), geometry->getVertexPositionIndices().end(), clusteredScene.vertexPositionIndices.begin() + indexOffset);
			std::copy(geometry->getVertexNormalIndices().begin(), geometry->getVertexNormalIndices().end(), clusteredScene.vertexNormalIndices.begin() + indexOffset);
			std::copy(geometry->getVertexTextureCoordinateIndices().begin(), geometry->getVertexTextureCoordinateIndices().end(), clusteredScene.vertexTextureCoordinateIndices.begin() + indexOffset);

			vertexOffset += clusteredGeometry.vertexCount;
			indexOffset += clusteredGeometry.clusterCount * CLUSTER_INDEX_COUNT;
			vertexPositionComponentOffset += static_cast<uint32>(geometry->getVertexPositionComponents().count());
			vertexNormalComponentOffset += static_cast<uint32>(geometry->getVertexNormalComponents().count());
			vertexTextureCoordinateComponentOffset += static_cast<uint32>(geometry->getVertexTextureCoordinateComponents().count());
		}

		clusterOffset = 0;

		for (uint32 instanceIndex = 0; instanceIndex < scene.getInstanceCount(); instanceIndex += 1) {
			const RigidBody3D& instance = scene.getInstance(instanceIndex);
			const ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[instance.geometryIndex];

			ClusteredInstance& clusteredInstance = clusteredScene.instances[instanceIndex];
			clusteredInstance.transform = instance.transform;
			clusteredInstance.geometryIndex = instance.geometryIndex;
			clusteredInstance.clusterOffset = clusteredGeometry.clusterOffset - clusterOffset;

			for (uint32 clusterIndex = 0; clusterIndex < clusteredGeometry.clusterCount; clusterIndex += 1) {
				Cluster& cluster = clusteredScene.clusters[clusterOffset + clusterIndex];
				cluster.instanceIndex = instanceIndex;
			}

			clusterOffset += clusteredGeometry.clusterCount;
		}

		return clusteredScene;
	}
}