// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<Scene3DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Scene3DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();

		const uint8 indexByteSize = stream.read<uint8>();

		ENGINE_ASSERT(indexByteSize == 4, "Only 32-bit indices are supported.");

		const uint32 vertexCount = stream.read<uint32>();
		const uint32 staticVertexCount = stream.read<uint32>();
		const uint32 bakedVertexCount = stream.read<uint32>();

		const uint32 triangleCount = stream.read<uint32>();
		const uint32 indexCount = triangleCount * 3;
		const uint32 bakedTriangleCount = stream.read<uint32>();
		const uint32 bakedIndexCount = bakedTriangleCount * 3;
		const uint32 lightmapCoordinateCount = stream.read<uint32>();

		asset.geometryRanges[0] = {0, vertexCount * sizeof(Vertex3D)};
		asset.geometryRanges[1] = {asset.geometryRanges[0].count, indexCount * sizeof(uint32)};
		asset.geometryRanges[2] = {asset.geometryRanges[1].offset + asset.geometryRanges[1].count, lightmapCoordinateCount * sizeof(uint32)};

		const uint64 geometrySize = asset.geometryRanges[2].offset + asset.geometryRanges[2].count;

		asset.geometryData = sequence<uint8>(geometrySize);

		const uint32 staticInstanceCount = stream.read<uint32>();
		const uint32 geometryCount = stream.read<uint32>();

		asset.geometryIndexRanges = sequence<range<uint32, uint32>>(geometryCount);

		const uint32 materialGeometryCount = stream.read<uint32>();

		asset.materialGeometries = sequence<MaterialGeometry>(materialGeometryCount);

		const uint32 materialCount = stream.read<uint32>();

		asset.materials = sequence<Material>(materialCount);

		const uint32 staticMaterialShaderCount = stream.read<uint32>();
		const uint32 dynamicMaterialShaderCount = stream.read<uint32>();

		asset.dynamicMaterialShaderOffset = staticMaterialShaderCount;
		asset.materialShaders = sequence<MaterialShader>(staticMaterialShaderCount + dynamicMaterialShaderCount);

		asset.entityCount = stream.read<uint32>();
		asset.instances = sequence<Instance3D>(staticInstanceCount + asset.entityCount);
		asset.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(asset.entityCount);
		asset.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(asset.entityCount);

		const uint32 lightmapAtlasImageAssetIdentifier = stream.read<uint32>();

		if (lightmapAtlasImageAssetIdentifier != max<uint32>()) {
			asset.lightmapAtlasImageAsset = assetLibrary->getAsset<ImageAsset>(lightmapAtlasImageAssetIdentifier);
		}

		asset.skyColor[0] = stream.read<float32>();
		asset.skyColor[1] = stream.read<float32>();
		asset.skyColor[2] = stream.read<float32>();

		std::copy_n(stream.getBytePosition(), bakedVertexCount * sizeof(Vertex3D), &asset.geometryData[asset.geometryRanges[0].offset]);

		stream.jump(bakedVertexCount * sizeof(Vertex3D));

		std::copy_n(stream.getBytePosition(), bakedIndexCount * sizeof(uint32), &asset.geometryData[asset.geometryRanges[1].offset]);

		stream.jump(bakedIndexCount * sizeof(uint32));

		if (asset.geometryRanges[2].count > 0) {
			std::copy_n(stream.getBytePosition(), asset.geometryRanges[2].count, &asset.geometryData[asset.geometryRanges[2].offset]);

			stream.jump(asset.geometryRanges[2].count);
		}

		uint32 instanceIndex = 0;

		while (instanceIndex < staticInstanceCount) {
			const svec3<float32> position = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			const svec3<float32> rotation = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			const float32 scale = stream.read<float32>();
			const quat<float32> orientation = normalized(
				quat<float32>::axisAngle(UP, rotation[1]) *
				quat<float32>::axisAngle(RIGHT, rotation[0]) *
				quat<float32>::axisAngle(FORWARD, rotation[2])
			);

			Instance3D& instance = asset.instances[instanceIndex];
			instance.worldTransform = (
				mat4x4<float32>::scaling({scale, scale, scale}) *
				mat4x4<float32>(orientation) *
				mat4x4<float32>::translation(position)
			);
			instance.normalTransform = mat3x3<float32>::identity();

			instanceIndex += 1;
		}

		Vertex3D* vertices = pointer_cast<Vertex3D*>(&asset.geometryData[asset.geometryRanges[0].offset]);
		uint32* indices = pointer_cast<uint32*>(&asset.geometryData[asset.geometryRanges[1].offset]);
		uint32 vertexOffset = bakedVertexCount;
		uint32 indexOffset = bakedIndexCount;

		sequence<ref<Geometry3DAsset>> geometryAssets(geometryCount);
		uint32 geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			asset.geometryIndexRanges[geometryIndex] = stream.read<range<uint32, uint32>>();

			const uint32 geometryAssetIdentifier = stream.read<uint32>();

			if (geometryAssetIdentifier == 0) {
				geometryIndex += 1;

				continue;
			}

			ref<Geometry3DAsset>& geometryAsset = geometryAssets[geometryIndex];
			geometryAsset = assetLibrary->getAsset<Geometry3DAsset>(geometryAssetIdentifier);

			std::copy(geometryAsset->vertices.begin(), geometryAsset->vertices.end(), &vertices[vertexOffset]);

			uint32 indexIndex = 0;

			while (indexIndex < geometryAsset->indices.count()) {
				indices[indexOffset + indexIndex] = vertexOffset + geometryAsset->indices[indexIndex];

				indexIndex += 1;
			}

			indexOffset += indexIndex;
			vertexOffset += static_cast<uint32>(geometryAsset->vertices.count());

			if (geometryAsset->flags & GeometryAssetFlags::HAS_HULL) {
				asset.hullCount += 1;
				asset.hullEdgeCount += static_cast<uint32>(geometryAsset->hullEdges.count());
				asset.hullFaceCount += static_cast<uint32>(geometryAsset->hullFaces.count());
				asset.hullFaceEdgeIndexCount += static_cast<uint16>(geometryAsset->hullFaceEdgeIndices.count());
				asset.hullLineIndexCount += static_cast<uint32>(geometryAsset->hullLineIndices.count());
			}

			geometryIndex += 1;
		}

		for (MaterialGeometry& materialGeometry : asset.materialGeometries) {
			materialGeometry.instanceRange = stream.read<range<uint32, uint32>>();
			materialGeometry.geometryIndex = stream.read<uint32>();
		}

		uint32 materialGeometryOffset = 0;

		for (Material& material : asset.materials) {
			const uint32 materialAssetIdentifier = stream.read<uint32>();

			material.asset = assetLibrary->getAsset<MaterialAsset>(materialAssetIdentifier);
			material.geometryRange.offset = materialGeometryOffset;
			material.geometryRange.count = stream.read<uint32>();

			materialGeometryOffset += material.geometryRange.count;
		}

		for (MaterialShader& materialShader : asset.materialShaders) {
			stream.jump(sizeof(AssetSignature));

			materialShader.shaderModuleAsset = decodeAsset<ShaderModuleAsset>(stream, assetLibrary);
			materialShader.materialRange = stream.read<range<uint32, uint32>>();
		}

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : asset.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component = stream.read<GeometryComponent>();
		}

		for (EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : asset.rigidBodyMappings) {
			rigidBodyMapping.entity = stream.read<Entity>();

			RigidBody3DComponent& component = rigidBodyMapping.component;
			component.position = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			component.rotation = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			component.linearVelocity = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			component.angularVelocity = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			component.scale = stream.read<float32>();
			component.inverseMass = stream.read<float32>();
			component.inverseInertia = {
				stream.read<float32>(), stream.read<float32>(), stream.read<float32>(),
				stream.read<float32>(), stream.read<float32>(), stream.read<float32>(),
				stream.read<float32>(), stream.read<float32>(), stream.read<float32>(),
			};
			component.friction = stream.read<float32>();
			component.restitution = stream.read<float32>();

			component.updateOrientation();
			component.updateWorld({0.0_m, 0.0_m, 0.0_m});

			for (const EntityComponentMapping<GeometryComponent>& geometryMapping : asset.geometryMappings) {
				if (geometryMapping.entity != rigidBodyMapping.entity) {
					continue;
				}

				const ref<Geometry3DAsset> geometryAsset = geometryAssets[geometryMapping.component.geometryIndex];

				if (!(geometryAsset->flags & GeometryAssetFlags::HAS_HULL)) {
					continue;
				}

				asset.rigidBodyFacePlaneCount += static_cast<uint32>(geometryAsset->hullFaces.count());
			}

			Instance3D& instance = asset.instances[instanceIndex + rigidBodyMapping.entity];
			instance.worldTransform = component.world;
			instance.normalTransform = component.calculateNormal();
		}

		return createRef<Scene3DAsset>(asset);
	}
}