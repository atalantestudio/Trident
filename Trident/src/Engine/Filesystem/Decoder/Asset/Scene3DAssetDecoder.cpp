// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<Scene3DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Descriptor<Scene3DAsset> descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();

		const uint32 staticTriangleCount = stream.read<uint32>();
		const uint32 staticIndexCount = staticTriangleCount * 3;
		const uint32 staticVertexCount = stream.read<uint32>();
		const uint32 staticNormalCount = stream.read<uint32>();
		const uint32 staticTextureCoordinateCount = stream.read<uint32>();

		descriptor.staticIndexRange = {0, staticIndexCount * sizeof(StaticVertex3D)};
		descriptor.staticVertexRange = {descriptor.staticIndexRange.offset + descriptor.staticIndexRange.count, staticVertexCount * sizeof(svec3<float32>)};
		descriptor.staticNormalRange = {descriptor.staticVertexRange.offset + descriptor.staticVertexRange.count, staticNormalCount * sizeof(svec3<float32>)};
		descriptor.staticTextureCoordinateRange = {descriptor.staticNormalRange.offset + descriptor.staticNormalRange.count, staticTextureCoordinateCount * sizeof(svec2<float32>)};

		descriptor.dynamicVertexPositionCount = 0;
		descriptor.dynamicVertexNormalCount = 0;
		descriptor.dynamicVertexTextureCoordinateCount = 0;
		descriptor.dynamicTriangleTangentCount = 0;
		descriptor.dynamicVertexPositionIndexCount = 0;
		descriptor.dynamicVertexNormalIndexCount = 0;
		descriptor.dynamicVertexTextureCoordinateIndexCount = 0;
		descriptor.entityCount = stream.read<uint32>();
		descriptor.hullCount = 0;
		descriptor.hullEdgeCount = 0;
		descriptor.hullFaceCount = 0;
		descriptor.hullFaceEdgeIndexCount = 0;
		descriptor.rigidBodyFacePlaneCount = 0;
		descriptor.staticGeometryBuffer = sequence<uint8>(descriptor.staticIndexRange.count + descriptor.staticVertexRange.count + descriptor.staticNormalRange.count + descriptor.staticTextureCoordinateRange.count);
		descriptor.geometryAssets = sequence<ref<GeometryAsset>>(stream.read<uint32>());
		descriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(stream.read<uint32>());
		descriptor.materialMappings = sequence<EntityComponentMapping<MaterialComponent>>(stream.read<uint32>());
		descriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(stream.read<uint32>());

		const uint32 lightmapAtlasImageAssetIdentifier = stream.read<uint32>();

		if (lightmapAtlasImageAssetIdentifier != max<uint32>()) {
			descriptor.lightmapAtlasImageAsset = assetLibrary->getAsset<ImageAsset>(lightmapAtlasImageAssetIdentifier);
		}

		descriptor.materialShaders = sequence<MaterialShader>(stream.read<uint32>());
		descriptor.materialAssets = sequence<ref<MaterialAsset>>(stream.read<uint32>());
		descriptor.staticMaterials = sequence<StaticMaterial3D>(descriptor.materialAssets.count());

		StaticVertex3D* staticIndex = pointer_cast<StaticVertex3D*>(&descriptor.staticGeometryBuffer[descriptor.staticIndexRange.offset]);
		svec3<float32>* staticVertex = pointer_cast<svec3<float32>*>(&descriptor.staticGeometryBuffer[descriptor.staticVertexRange.offset]);
		svec3<float32>* staticNormal = pointer_cast<svec3<float32>*>(&descriptor.staticGeometryBuffer[descriptor.staticNormalRange.offset]);
		svec2<float32>* staticTextureCoordinate = pointer_cast<svec2<float32>*>(&descriptor.staticGeometryBuffer[descriptor.staticTextureCoordinateRange.offset]);
		uint32 index = 0;

		while (index < staticIndexCount) {
			staticIndex->lightmapCoordinate = stream.read<svec2<float32>>();
			staticIndex->vertexIndex = stream.read<uint32>();
			staticIndex->normalIndex = stream.read<uint32>();
			staticIndex->textureCoordinateIndex = stream.read<uint32>();
			staticIndex += 1;

			index += 1;
		}

		index = 0;

		while (index < staticVertexCount) {
			staticVertex->x = stream.read<float32>();
			staticVertex->y = stream.read<float32>();
			staticVertex->z = stream.read<float32>();
			staticVertex += 1;

			index += 1;
		}

		index = 0;

		while (index < staticNormalCount) {
			staticNormal->x = stream.read<float32>();
			staticNormal->y = stream.read<float32>();
			staticNormal->z = stream.read<float32>();
			staticNormal += 1;

			index += 1;
		}

		index = 0;

		while (index < staticTextureCoordinateCount) {
			staticTextureCoordinate->x = stream.read<float32>();
			staticTextureCoordinate->y = stream.read<float32>();
			staticTextureCoordinate += 1;

			index += 1;
		}

		for (MaterialShader& materialShader : descriptor.materialShaders) {
			materialShader.shaderModuleAsset = decodeAsset<ShaderModuleAsset>(stream, assetLibrary);
			materialShader.materialAssetRange.offset = stream.read<uint32>();
			materialShader.materialAssetRange.count = stream.read<uint32>();
		}

		for (ref<MaterialAsset>& materialAsset : descriptor.materialAssets) {
			const uint32 identifier = stream.read<uint32>();

			materialAsset = assetLibrary->getAsset<MaterialAsset>(identifier);
		}

		for (StaticMaterial3D& staticMaterial : descriptor.staticMaterials) {
			staticMaterial.index = stream.read<uint32>();
			staticMaterial.indexRange = stream.read<range<uint32, uint32>>();
		}

		for (ref<GeometryAsset>& geometryAsset : descriptor.geometryAssets) {
			const uint32 identifier = stream.read<uint32>();

			geometryAsset = assetLibrary->getAsset<GeometryAsset>(identifier);

			descriptor.dynamicVertexPositionCount += geometryAsset->getVertexPositionComponents().count() / 3;
			descriptor.dynamicVertexNormalCount += geometryAsset->getVertexNormalComponents().count() / 3;
			descriptor.dynamicVertexTextureCoordinateCount += geometryAsset->getVertexTextureCoordinateComponents().count() / 2;
			descriptor.dynamicTriangleTangentCount += geometryAsset->getTriangleTangentComponents().count() / 3;
			descriptor.dynamicVertexPositionIndexCount += geometryAsset->getVertexPositionIndices().count();
			descriptor.dynamicVertexNormalIndexCount += geometryAsset->getVertexNormalIndices().count();
			descriptor.dynamicVertexTextureCoordinateIndexCount += geometryAsset->getVertexTextureCoordinateIndices().count();

			if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
				descriptor.hullCount += 1;
				descriptor.hullEdgeCount += static_cast<uint32>(geometryAsset->getHull3DEdges().count());
				descriptor.hullFaceCount += static_cast<uint32>(geometryAsset->getHull3DFaces().count());
				descriptor.hullFaceEdgeIndexCount += static_cast<uint16>(geometryAsset->getHull3DFaceEdgeIndices().count());
				descriptor.hullLineIndexCount += static_cast<uint32>(geometryAsset->getHull3DLineIndices().count());
			}
		}

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : descriptor.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component.geometryIndex = stream.read<uint32>();
		}

		for (EntityComponentMapping<MaterialComponent>& materialMapping : descriptor.materialMappings) {
			materialMapping.entity = stream.read<Entity>();
			materialMapping.component.materialIndex = stream.read<uint32>();
		}

		for (EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : descriptor.rigidBodyMappings) {
			rigidBodyMapping.entity = stream.read<Entity>();
			rigidBodyMapping.component.position = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.rotation = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.linearVelocity = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.angularVelocity = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.scale = stream.read<float32>();
			rigidBodyMapping.component.inverseMass = stream.read<float32>();
			rigidBodyMapping.component.inverseInertia = {
				vec3<float32>(stream.read<svec3<float32>>()),
				vec3<float32>(stream.read<svec3<float32>>()),
				vec3<float32>(stream.read<svec3<float32>>()),
			};
			rigidBodyMapping.component.friction = stream.read<float32>();
			rigidBodyMapping.component.restitution = stream.read<float32>();

			for (const EntityComponentMapping<GeometryComponent>& geometryMapping : descriptor.geometryMappings) {
				if (geometryMapping.entity != rigidBodyMapping.entity) {
					continue;
				}

				const ref<GeometryAsset>& geometryAsset = descriptor.geometryAssets[geometryMapping.component.geometryIndex];

				if (!(geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL)) {
					continue;
				}

				descriptor.rigidBodyFacePlaneCount += static_cast<uint32>(geometryAsset->getHull3DFaces().count());
			}
		}

		return createRef<Scene3DAsset>(descriptor);
	}
}