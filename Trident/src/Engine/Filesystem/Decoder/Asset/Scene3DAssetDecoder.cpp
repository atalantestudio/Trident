// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<Scene3DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, const AssetLibrary* assetLibrary) {
		Descriptor<Scene3DAsset> descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();

		const uint32 staticTriangleCount = stream.read<uint32>();
		const uint32 staticVertexCount = stream.read<uint32>();
		const uint32 staticNormalCount = stream.read<uint32>();
		const uint32 staticTextureCoordinateCount = stream.read<uint32>();
		const uint32 staticLightmapCoordinateCount = stream.read<uint32>();

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
		descriptor.staticTriangles = sequence<StaticTriangle3D>(staticTriangleCount);
		descriptor.staticVertices = sequence<svec3<float32>>(staticVertexCount);
		descriptor.staticNormals = sequence<svec3<float32>>(staticNormalCount);
		descriptor.staticTextureCoordinates = sequence<svec2<float32>>(staticTextureCoordinateCount);
		descriptor.staticLightmapCoordinates = sequence<svec2<float32>>(staticLightmapCoordinateCount);

		const uint32 geometryAssetCount = stream.read<uint32>();
		const uint32 geometryMappingCount = stream.read<uint32>();
		const uint32 materialMappingCount = stream.read<uint32>();
		const uint32 rigidBodyMappingCount = stream.read<uint32>();

		descriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);
		descriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);
		descriptor.materialMappings = sequence<EntityComponentMapping<MaterialComponent>>(materialMappingCount);
		descriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(rigidBodyMappingCount);

		for (StaticTriangle3D& triangle : descriptor.staticTriangles) {
			uint32 vertexIndex = 0;

			while (vertexIndex < 3) {
				StaticVertex3D& vertex = triangle.vertices[vertexIndex];
				vertex.vertexIndex = stream.read<uint32>();
				vertex.textureCoordinateIndex = stream.read<uint32>();
				vertex.lightmapCoordinateIndex = stream.read<uint32>();

				vertexIndex += 1;
			}

			triangle.normalIndex = stream.read<uint32>();
		}

		for (svec3<float32>& vertex : descriptor.staticVertices) {
			vertex = stream.read<svec3<float32>>();
		}

		for (svec3<float32>& normal : descriptor.staticNormals) {
			normal = stream.read<svec3<float32>>();
		}

		for (svec2<float32>& textureCoordinate : descriptor.staticTextureCoordinates) {
			textureCoordinate = stream.read<svec2<float32>>();
		}

		for (svec2<float32>& lightmapCoordinate : descriptor.staticLightmapCoordinates) {
			lightmapCoordinate = stream.read<svec2<float32>>();
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