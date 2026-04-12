// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	Scene3D::Scene3D(ref<Scene3DAsset> sceneAsset, uint64 minStorageBufferOffsetAlignment) :
		Scene({
			.staticVertexCount = sceneAsset->getStaticVertexCount(),
			.staticIndexCount = sceneAsset->getStaticIndexCount(),
			.dynamicVertexCount = sceneAsset->getDynamicVertexPositionCount() + sceneAsset->getDynamicVertexNormalCount() + sceneAsset->getDynamicVertexTextureCoordinateCount(),
			.dynamicIndexCount = sceneAsset->getDynamicVertexPositionIndexCount() + sceneAsset->getDynamicVertexNormalIndexCount() + sceneAsset->getDynamicVertexTextureCoordinateIndexCount()
		}),
		vertexPositionComponentRange(0, 0),
		vertexNormalComponentRange(0, 0),
		vertexTextureCoordinateComponentRange(0, 0),
		triangleTangentComponentRange(0, 0),
		vertexPositionIndexRange(0, 0),
		vertexNormalIndexRange(0, 0),
		vertexTextureCoordinateIndexRange(0, 0)
	{
		loadGeometries(sceneAsset, minStorageBufferOffsetAlignment);
		loadEntityComponentMappings(sceneAsset);
	}

	void Scene3D::loadGeometries(ref<Scene3DAsset> sceneAsset, uint64 minStorageBufferOffsetAlignment) {
		const sequence<uint8>& staticBuffer = sceneAsset->getStaticBuffer();
		const sequence<ref<GeometryAsset>>& geometryAssets = sceneAsset->getGeometryAssets();
		const uint64 vertexComponentCount = (
			sceneAsset->getStaticVertexCount() * 3 +
			sceneAsset->getDynamicVertexPositionCount() * 3 +
			sceneAsset->getDynamicVertexNormalCount() * 3 +
			sceneAsset->getDynamicVertexTextureCoordinateCount() * 2
		);

		// TODO: Include static ranges.
		vertexPositionComponentRange.count = ceil(sceneAsset->getDynamicVertexPositionCount() * sizeof(svec3<float32>), minStorageBufferOffsetAlignment);
		vertexNormalComponentRange.count = ceil(sceneAsset->getDynamicVertexNormalCount() * sizeof(svec3<float32>), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateComponentRange.count = ceil(sceneAsset->getDynamicVertexTextureCoordinateCount() * sizeof(svec2<float32>), minStorageBufferOffsetAlignment);
		triangleTangentComponentRange.count = ceil(sceneAsset->getDynamicTriangleTangentCount() * sizeof(svec3<float32>), minStorageBufferOffsetAlignment);
		vertexPositionIndexRange.count = ceil(sceneAsset->getDynamicVertexPositionIndexCount() * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexNormalIndexRange.count = ceil(sceneAsset->getDynamicVertexNormalIndexCount() * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateIndexRange.count = ceil(sceneAsset->getDynamicVertexTextureCoordinateIndexCount() * sizeof(uint32), minStorageBufferOffsetAlignment);

		range<uint64, uint64>* ranges = &vertexPositionComponentRange;
		uint64 dataBufferSize = ceil(sceneAsset->getStaticVertexCount() * sizeof(svec3<float32>), minStorageBufferOffsetAlignment);

		// Propagate the range offset.
		while (ranges <= &vertexTextureCoordinateIndexRange) {
			ranges[1].offset = ranges[0].offset + ranges[0].count;
			dataBufferSize += ranges[0].count;

			ranges += 1;
		}

		dataBuffer = sequence<uint8>(dataBufferSize);
		geometries = sequence<Geometry3D>(geometryAssets.count());

		std::copy(staticBuffer.begin(), staticBuffer.end(), &dataBuffer[0]);

		uint64 byteOffset = staticBuffer.count() + vertexPositionComponentRange.offset;

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

		byteOffset = vertexPositionIndexRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<uint32>& vertexPositionIndices = geometryAsset->getVertexPositionIndices();

			std::copy(vertexPositionIndices.begin(), vertexPositionIndices.end(), pointer_cast<uint32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexPositionIndices.count() * sizeof(uint32);
		}

		byteOffset = vertexNormalIndexRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<uint32>& vertexNormalIndices = geometryAsset->getVertexNormalIndices();

			std::copy(vertexNormalIndices.begin(), vertexNormalIndices.end(), pointer_cast<uint32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexNormalIndices.count() * sizeof(uint32);
		}

		byteOffset = vertexTextureCoordinateIndexRange.offset;

		for (const ref<GeometryAsset>& geometryAsset : geometryAssets) {
			const sequence<uint32>& vertexTextureCoordinateIndices = geometryAsset->getVertexTextureCoordinateIndices();

			std::copy(vertexTextureCoordinateIndices.begin(), vertexTextureCoordinateIndices.end(), pointer_cast<uint32*>(&dataBuffer[byteOffset]));

			byteOffset += vertexTextureCoordinateIndices.count() * sizeof(uint32);
		}

		// NOTE: The offsets in these ranges are relative to the start of the corresponding descriptor buffer.
		range<uint32, uint32> geometryVertexPositionComponentRange = {0, 0};
		range<uint32, uint32> geometryVertexNormalComponentRange = {0, 0};
		range<uint32, uint32> geometryVertexTextureCoordinateComponentRange = {0, 0};
		range<uint32, uint32> geometryTriangleTangentComponentRange = {0, 0};
		range<uint32, uint32> geometryVertexPositionIndexRange = {0, 0};
		range<uint32, uint32> geometryVertexNormalIndexRange = {0, 0};
		range<uint32, uint32> geometryVertexTextureCoordinateIndexRange = {0, 0};

		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const ref<GeometryAsset> geometryAsset = geometryAssets[geometryIndex];

			geometryVertexPositionComponentRange.count = static_cast<uint32>(geometryAsset->getVertexPositionComponents().count());
			geometryVertexNormalComponentRange.count = static_cast<uint32>(geometryAsset->getVertexNormalComponents().count());
			geometryVertexTextureCoordinateComponentRange.count = static_cast<uint32>(geometryAsset->getVertexTextureCoordinateComponents().count());
			geometryTriangleTangentComponentRange.count = static_cast<uint32>(geometryAsset->getTriangleTangentComponents().count());
			geometryVertexPositionIndexRange.count = static_cast<uint32>(geometryAsset->getVertexPositionIndices().count());
			geometryVertexNormalIndexRange.count = static_cast<uint32>(geometryAsset->getVertexNormalIndices().count());
			geometryVertexTextureCoordinateIndexRange.count = static_cast<uint32>(geometryAsset->getVertexTextureCoordinateIndices().count());

			geometries[geometryIndex] = Geometry3D({
				.triangleCount = geometryVertexPositionIndexRange.count / 3,
				.vertexPositionComponentRange = geometryVertexPositionComponentRange,
				.vertexNormalComponentRange = geometryVertexNormalComponentRange,
				.vertexTextureCoordinateComponentRange = geometryVertexTextureCoordinateComponentRange,
				.triangleTangentComponentOffset = geometryTriangleTangentComponentRange.offset,
				.vertexPositionIndexOffset = geometryVertexPositionIndexRange.offset,
				.vertexNormalIndexOffset = geometryVertexNormalIndexRange.offset,
				.vertexTextureCoordinateIndexOffset = geometryVertexTextureCoordinateIndexRange.offset,
			});

			geometryVertexPositionComponentRange.offset += geometryVertexPositionComponentRange.count;
			geometryVertexNormalComponentRange.offset += geometryVertexNormalComponentRange.count;
			geometryVertexTextureCoordinateComponentRange.offset += geometryVertexTextureCoordinateComponentRange.count;
			geometryTriangleTangentComponentRange.offset += geometryTriangleTangentComponentRange.count;
			geometryVertexPositionIndexRange.offset += geometryVertexPositionIndexRange.count;
			geometryVertexNormalIndexRange.offset += geometryVertexNormalIndexRange.count;
			geometryVertexTextureCoordinateIndexRange.offset += geometryVertexTextureCoordinateIndexRange.count;

			geometryIndex += 1;
		}
	}

	void Scene3D::loadEntityComponentMappings(ref<Scene3DAsset> sceneAsset) {
		ecs.registerComponentPool<GeometryComponent>();
		ecs.registerComponentPool<MaterialComponent>();

		// TODO(ecs/scene-components): Transform3DComponent for static meshes?
		ecs.registerComponentPool<RigidBody3DComponent>();

		Entity entity = 0;

		while (entity < sceneAsset->getEntityCount()) {
			ecs.createEntity();

			entity += 1;
		}

		for (const EntityComponentMapping<GeometryComponent> geometryMapping : sceneAsset->getGeometryMappings()) {
			ecs.setComponent<GeometryComponent>(geometryMapping.entity, geometryMapping.component);
		}

		for (const EntityComponentMapping<MaterialComponent> materialMapping : sceneAsset->getMaterialMappings()) {
			ecs.setComponent<MaterialComponent>(materialMapping.entity, materialMapping.component);
		}

		for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : sceneAsset->getRigidBodyMappings()) {
			ecs.setComponent<RigidBody3DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);

			// TODO(debug-only)
			ecs.getComponent<RigidBody3DComponent>(rigidBodyMapping.entity).updateWorld({0.0_m, 0.0_m, 0.0_m});
		}
	}
}