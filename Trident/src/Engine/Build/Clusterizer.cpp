// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Geometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Build/ClusteredGeometry.hpp"
#include "Engine/Build/ClusteredInstance.hpp"
#include "Engine/Build/Clusterizer.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	ClusteredScene clusterizeScene(const Scene3D* scene) {
		const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
		const ComponentPool<MaterialComponent>& materialComponentPool = scene->ecs.getComponentPool<MaterialComponent>();
		const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();
		const view<uint8> dataBuffer = scene->getDataBuffer();
		const sequence<Geometry3D>& geometries = scene->getGeometries();

		ClusteredScene clusteredScene{};
		clusteredScene.geometries = sequence<ClusteredGeometry>(geometries.count());
		clusteredScene.materialShaders = scene->getMaterialShaders();
		clusteredScene.materials = scene->getMaterials();

		uint64 totalVertexPositionComponentCount = 0;
		uint64 totalVertexNormalComponentCount = 0;
		uint64 totalVertexTextureCoordinateComponentCount = 0;
		uint64 totalTriangleTangentComponentCount = 0;
		uint32 totalIndexCount = 0;
		uint32 totalClusterCount = 0;
		uint32 totalInstanceCount = 0;

		uint32 clusterOffset = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const Geometry3D& geometry = geometries[geometryIndex];
			const uint64 indexCount = geometry.getVertexPositionIndexRange().count;

			// There should be as many position indices as there are normal and texture coordinate indices.
			DEBUG_ASSERT(indexCount == geometry.getVertexNormalIndexRange().count && indexCount == geometry.getVertexTextureCoordinateIndexRange().count);

			const uint32 clusterCount = static_cast<uint32>(ceil(static_cast<float32>(indexCount) / CLUSTER_INDEX_COUNT));

			uint32 instanceCount = 0;

			// TODO: Optimize.
			// Count the number of instances using this geometry.
			{
				uint32 rigidBodyComponentIndex = 0;

				while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
					const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
					const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];

					if (geometryComponent.geometryIndex == geometryIndex) {
						instanceCount += 1;
					}

					rigidBodyComponentIndex += 1;
				}
			}

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.vertexCount = static_cast<uint32>(geometry.getVertexPositionComponentRange().count / 3);
			clusteredGeometry.clusterCount = clusterCount;
			clusteredGeometry.clusterOffset = clusterOffset;
			clusteredGeometry.instanceCount = instanceCount;

			totalVertexPositionComponentCount += geometry.getVertexPositionComponentRange().count;
			totalVertexNormalComponentCount += geometry.getVertexNormalComponentRange().count;
			totalVertexTextureCoordinateComponentCount += geometry.getVertexTextureCoordinateComponentRange().count;
			totalTriangleTangentComponentCount += geometry.getTriangleTangentComponentRange().count;
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
		clusteredScene.triangleTangentComponents = sequence<float32>(totalTriangleTangentComponentCount);
		clusteredScene.vertexPositionIndices = sequence<uint32>(totalIndexCount);
		clusteredScene.vertexNormalIndices = sequence<uint32>(totalIndexCount);
		clusteredScene.vertexTextureCoordinateIndices = sequence<uint32>(totalIndexCount);

		uint64 vertexPositionComponentOffset = 0;
		uint64 vertexNormalComponentOffset = 0;
		uint64 vertexTextureCoordinateComponentOffset = 0;
		uint64 triangleTangentComponentOffset = 0;
		uint32 vertexOffset = 0;
		uint32 vertexTextureCoordinateOffset = 0;

		// Index offset relative to the parent cluster. Includes degenerate indices.
		uint32 indexOffset = 0;

		geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const Geometry3D& geometry = geometries[geometryIndex];

			ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryIndex];
			clusteredGeometry.vertexOffset = vertexOffset;
			clusteredGeometry.vertexTextureCoordinateOffset = vertexTextureCoordinateOffset;

			std::copy_n(
				pointer_cast<const float32*>(&dataBuffer[scene->getVertexPositionComponentRange().offset + geometry.getVertexPositionComponentRange().offset * sizeof(float32)]),
				geometry.getVertexPositionComponentRange().count,
				&clusteredScene.vertexPositionComponents[vertexPositionComponentOffset]
			);

			std::copy_n(
				pointer_cast<const float32*>(&dataBuffer[scene->getVertexNormalComponentRange().offset + geometry.getVertexNormalComponentRange().offset * sizeof(float32)]),
				geometry.getVertexNormalComponentRange().count,
				&clusteredScene.vertexNormalComponents[vertexNormalComponentOffset]
			);

			std::copy_n(
				pointer_cast<const float32*>(&dataBuffer[scene->getVertexTextureCoordinateComponentRange().offset + geometry.getVertexTextureCoordinateComponentRange().offset * sizeof(float32)]),
				geometry.getVertexTextureCoordinateComponentRange().count,
				&clusteredScene.vertexTextureCoordinateComponents[vertexTextureCoordinateComponentOffset]
			);

			std::copy_n(
				pointer_cast<const float32*>(&dataBuffer[scene->getTriangleTangentComponentRange().offset + geometry.getTriangleTangentComponentRange().offset * sizeof(float32)]),
				geometry.getTriangleTangentComponentRange().count,
				&clusteredScene.triangleTangentComponents[triangleTangentComponentOffset]
			);

			std::copy_n(
				pointer_cast<const uint32*>(&dataBuffer[scene->getVertexPositionIndexRange().offset + geometry.getVertexPositionIndexRange().offset * sizeof(uint32)]),
				geometry.getVertexPositionIndexRange().count,
				&clusteredScene.vertexPositionIndices[indexOffset]
			);

			std::copy_n(
				pointer_cast<const uint32*>(&dataBuffer[scene->getVertexNormalIndexRange().offset + geometry.getVertexNormalIndexRange().offset * sizeof(uint32)]),
				geometry.getVertexNormalIndexRange().count,
				&clusteredScene.vertexNormalIndices[indexOffset]
			);

			std::copy_n(
				pointer_cast<const uint32*>(&dataBuffer[scene->getVertexTextureCoordinateIndexRange().offset + geometry.getVertexTextureCoordinateIndexRange().offset * sizeof(uint32)]),
				geometry.getVertexTextureCoordinateIndexRange().count,
				&clusteredScene.vertexTextureCoordinateIndices[indexOffset]
			);

			vertexPositionComponentOffset += geometry.getVertexPositionComponentRange().count;
			vertexNormalComponentOffset += geometry.getVertexNormalComponentRange().count;
			vertexTextureCoordinateComponentOffset += geometry.getVertexTextureCoordinateComponentRange().count;
			triangleTangentComponentOffset += geometry.getTriangleTangentComponentRange().count;
			vertexOffset += clusteredGeometry.vertexCount;
			vertexTextureCoordinateOffset += static_cast<uint32>(geometry.getVertexTextureCoordinateComponentRange().count) / 2;
			indexOffset += clusteredGeometry.clusterCount * CLUSTER_INDEX_COUNT;

			geometryIndex += 1;
		}

		uint32 clusterIndex;

		clusterOffset = 0;

		uint32 rigidBodyComponentIndex = 0;

		while (rigidBodyComponentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[rigidBodyComponentIndex];
			const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[rigidBodyComponentIndex];
			const GeometryComponent geometryComponent = geometryComponentPool.components[geometryComponentPool.entityToComponentIndex[entity]];
			const MaterialComponent materialComponent = materialComponentPool.components[materialComponentPool.entityToComponentIndex[entity]];

			const ClusteredGeometry& clusteredGeometry = clusteredScene.geometries[geometryComponent.geometryIndex];

			ClusteredInstance& clusteredInstance = clusteredScene.instances[rigidBodyComponentIndex];
			clusteredInstance.world = rigidBodyComponent.world;
			clusteredInstance.normal = rigidBodyComponent.calculateNormal();
			clusteredInstance.geometryIndex = geometryComponent.geometryIndex;
			clusteredInstance.materialShaderIndex = scene->getMaterialShaderIndex(materialComponent.materialIndex);
			clusteredInstance.materialIndex = materialComponent.materialIndex;
			clusteredInstance.clusterOffset = clusteredGeometry.clusterOffset - clusterOffset;

			clusterIndex = 0;

			while (clusterIndex < clusteredGeometry.clusterCount) {
				Cluster& cluster = clusteredScene.clusters[clusterOffset + clusterIndex];
				cluster.instanceIndex = rigidBodyComponentIndex;

				clusterIndex += 1;
			}

			clusterOffset += clusteredGeometry.clusterCount;
			rigidBodyComponentIndex += 1;
		}

		return clusteredScene;
	}
}