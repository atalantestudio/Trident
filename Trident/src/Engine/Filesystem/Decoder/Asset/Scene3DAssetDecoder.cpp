// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/Scene3DAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	Scene3DAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Scene3DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.skyColor = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};

		const uint32 skyboxAssetIdentifier = stream.read<uint32>();

		if (skyboxAssetIdentifier != max<uint32>()) {
			asset.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>(skyboxAssetIdentifier);
		}

		const uint8 indexByteSize = stream.read<uint8>();

		ENGINE_ASSERT(indexByteSize == 4, "Only 32-bit indices are supported.");

		const uint32 vertexCount = stream.read<uint32>();
		const uint32 staticVertexCount = stream.read<uint32>();
		const uint32 bakedVertexCount = stream.read<uint32>();
		const uint32 triangleCount = stream.read<uint32>();
		const uint32 indexCount = triangleCount * 3;
		const uint32 bakedTriangleCount = stream.read<uint32>();
		const uint32 bakedIndexCount = bakedTriangleCount * 3;
		const uint32 faceLineIndexCount = stream.read<uint32>();
		const uint32 lightmapCoordinateCount = stream.read<uint32>();

		asset.vertexRange = {0, vertexCount * sizeof(Vertex3D)};
		asset.indexRange = {asset.vertexRange.count, indexCount * sizeof(uint32)};
		asset.faceLineIndexRange = {asset.indexRange.offset + asset.indexRange.count, faceLineIndexCount * sizeof(uint32)};
		asset.lightmapCoordinateRange = {asset.faceLineIndexRange.offset + asset.faceLineIndexRange.count, lightmapCoordinateCount * sizeof(uint32)};

		const uint64 geometrySize = asset.lightmapCoordinateRange.offset + asset.lightmapCoordinateRange.count;

		asset.geometryData = sequence<uint8>(geometrySize);

		const uint32 staticInstanceCount = stream.read<uint32>();
		const uint32 geometryCount = stream.read<uint32>();

		asset.geometryAssets = sequence<ref<Geometry3DAsset>>(geometryCount);
		asset.geometryIndexRanges = sequence<range<uint32, uint32>>(geometryCount);

		const uint32 staticMaterialShaderCount = stream.read<uint32>();
		const uint32 dynamicMaterialShaderCount = stream.read<uint32>();

		asset.dynamicMaterialShaderOffset = staticMaterialShaderCount;
		asset.materialShaders = sequence<MaterialShader>(staticMaterialShaderCount + dynamicMaterialShaderCount);

		const uint32 materialGeometryCount = stream.read<uint32>();

		asset.materialGeometries = sequence<MaterialGeometry>(materialGeometryCount);

		const uint32 materialCount = stream.read<uint32>();

		asset.materials = sequence<Material>(materialCount);
		asset.entityCount = stream.read<uint32>();
		asset.instances = sequence<Instance3D>(staticInstanceCount + asset.entityCount);
		asset.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(asset.entityCount);
		asset.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(asset.entityCount);
		asset.sunDirection = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
		asset.sunColor = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};

		const uint32 pointLightCount = stream.read<uint32>();

		asset.pointLights = sequence<PointLight>(pointLightCount);

		const uint32 lightmapAtlasAssetIdentifier = stream.read<uint32>();

		if (lightmapAtlasAssetIdentifier != max<uint32>()) {
			asset.lightmapAtlasImageAsset = assetLibrary->getAsset<ImageAsset>(lightmapAtlasAssetIdentifier);
		}

		asset.spawnPosition = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
		asset.spawnDirection = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};

		std::copy_n(stream.getBytePosition(), bakedVertexCount * sizeof(Vertex3D), &asset.geometryData[asset.vertexRange.offset]);

		stream.jump(bakedVertexCount * sizeof(Vertex3D));

		std::copy_n(stream.getBytePosition(), bakedIndexCount * sizeof(uint32), &asset.geometryData[asset.indexRange.offset]);

		stream.jump(bakedIndexCount * sizeof(uint32));

		if (asset.faceLineIndexRange.count > 0) {
			std::copy_n(stream.getBytePosition(), faceLineIndexCount * sizeof(uint32), &asset.geometryData[asset.faceLineIndexRange.offset]);

			stream.jump(faceLineIndexCount * sizeof(uint32));
		}

		if (asset.lightmapCoordinateRange.count > 0) {
			std::copy_n(stream.getBytePosition(), asset.lightmapCoordinateRange.count, &asset.geometryData[asset.lightmapCoordinateRange.offset]);

			stream.jump(asset.lightmapCoordinateRange.count);
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

			// TODO(asset/scene/3d/instance): See https://marti.works/posts/post-normal-matrix-optimization/post.
			instance.normalTransform = transpose(inverse(mat3x3<float32>(
				instance.worldTransform[0][0], instance.worldTransform[0][1], instance.worldTransform[0][2],
				instance.worldTransform[1][0], instance.worldTransform[1][1], instance.worldTransform[1][2],
				instance.worldTransform[2][0], instance.worldTransform[2][1], instance.worldTransform[2][2]
			)));

			instanceIndex += 1;
		}

		Vertex3D* vertices = pointer_cast<Vertex3D*>(&asset.geometryData[asset.vertexRange.offset]);
		uint32* indices = pointer_cast<uint32*>(&asset.geometryData[asset.indexRange.offset]);
		uint32 vertexOffset = bakedVertexCount;
		uint32 indexOffset = bakedIndexCount;
		uint32 geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			asset.geometryIndexRanges[geometryIndex] = stream.read<range<uint32, uint32>>();

			const uint32 geometryAssetIdentifier = stream.read<uint32>();

			if (geometryAssetIdentifier == max<uint32>()) {
				geometryIndex += 1;

				continue;
			}

			ref<Geometry3DAsset>& geometryAsset = asset.geometryAssets[geometryIndex];
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

		for (PointLight& pointLight : asset.pointLights) {
			pointLight.color = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			pointLight.intensity = stream.read<float32>();
			pointLight.position = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
		}

		asset.geometryIndexToHullIndex = sequence<uint32>(asset.geometryIndexRanges.count());
		asset.hulls = sequence<Hull3D>(asset.entityCount);

		Entity entity = 0;

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : asset.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component = stream.read<GeometryComponent>();

			asset.geometryIndexToHullIndex[entity] = max<uint32>();

			entity += 1;
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

				const ref<Geometry3DAsset> geometryAsset = asset.geometryAssets[geometryMapping.component.geometryIndex];

				if (!(geometryAsset->flags & GeometryAssetFlags::HAS_HULL)) {
					continue;
				}

				asset.geometryIndexToHullIndex[rigidBodyMapping.entity] = rigidBodyMapping.entity;

				asset.rigidBodyFacePlaneCount += static_cast<uint32>(geometryAsset->hullFaces.count());

				Hull3D& hull = asset.hulls[rigidBodyMapping.entity];
				hull.centroid = geometryAsset->hullCentroid;
				hull.radius = geometryAsset->hullRadius;
				hull.vertices = geometryAsset->hullVertices;
				hull.edges = geometryAsset->hullEdges;
				hull.faces = geometryAsset->hullFaces;
				hull.faceEdgeIndices = geometryAsset->hullFaceEdgeIndices;

				// TODO(physics/hull/line-indices): Restore hull wireframe rendering.
				hull.lineIndexRange.offset = 0;
				hull.lineIndexRange.count = 0;

				hull.type = geometryAsset->hullType;

				break;
			}

			Instance3D& instance = asset.instances[instanceIndex + rigidBodyMapping.entity];
			instance.worldTransform = component.world;
			instance.normalTransform = component.calculateNormal();
		}

		return asset;
	}
}