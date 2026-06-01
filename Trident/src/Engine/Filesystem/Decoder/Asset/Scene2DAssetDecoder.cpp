// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<Scene2DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, const AssetLibrary* assetLibrary) {
		Descriptor<Scene2DAsset> descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();

		const uint32 staticTriangleCount = stream.read<uint32>();
		const uint32 staticVertexCount = stream.read<uint32>();

		// TODO(assets/scene/2d): This is the static normal count; don't include it in 2D scenes.
		stream.jump(sizeof(uint32));

		const uint32 staticTextureCoordinateCount = stream.read<uint32>();

		descriptor.dynamicVertexPositionCount = 0;
		descriptor.dynamicVertexTextureCoordinateCount = 0;
		descriptor.dynamicVertexPositionIndexCount = 0;
		descriptor.dynamicVertexTextureCoordinateIndexCount = 0;
		descriptor.entityCount = stream.read<uint32>();
		descriptor.hullCount = 0;
		descriptor.staticTriangles = sequence<StaticTriangle2D>(staticTriangleCount);
		descriptor.staticVertices = sequence<svec2<float32>>(staticVertexCount);
		descriptor.staticTextureCoordinates = sequence<svec2<float32>>(staticTextureCoordinateCount);

		const uint32 geometryAssetCount = stream.read<uint32>();
		const uint32 geometryMappingCount = stream.read<uint32>();
		const uint32 materialMappingCount = stream.read<uint32>();
		const uint32 rigidBodyMappingCount = stream.read<uint32>();

		descriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);
		descriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);
		descriptor.materialMappings = sequence<EntityComponentMapping<MaterialComponent>>(materialMappingCount);
		descriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody2DComponent>>(rigidBodyMappingCount);

		for (StaticTriangle2D& triangle : descriptor.staticTriangles) {
			triangle.aIndex = stream.read<uint32>();
			triangle.bIndex = stream.read<uint32>();
			triangle.cIndex = stream.read<uint32>();
			triangle.aUvIndex = stream.read<uint32>();
			triangle.bUvIndex = stream.read<uint32>();
			triangle.cUvIndex = stream.read<uint32>();
		}

		for (svec2<float32>& vertex : descriptor.staticVertices) {
			vertex = stream.read<svec2<float32>>();
		}

		for (svec2<float32>& textureCoordinate : descriptor.staticTextureCoordinates) {
			textureCoordinate = stream.read<svec2<float32>>();
		}

		for (ref<GeometryAsset>& geometryAsset : descriptor.geometryAssets) {
			const uint32 identifier = stream.read<uint32>();

			geometryAsset = assetLibrary->getAsset<GeometryAsset>(identifier);

			descriptor.dynamicVertexPositionCount += geometryAsset->getVertexPositionComponents().count() / 2;
			descriptor.dynamicVertexTextureCoordinateCount += geometryAsset->getVertexTextureCoordinateComponents().count() / 2;
			descriptor.dynamicVertexPositionIndexCount += geometryAsset->getVertexPositionIndices().count();
			descriptor.dynamicVertexTextureCoordinateIndexCount += geometryAsset->getVertexTextureCoordinateIndices().count();

			if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
				// TODO: Don't mix triangle and line indices?
				descriptor.dynamicVertexPositionIndexCount += geometryAsset->getHull2DLineIndices().count();

				descriptor.hullCount += 1;
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

		for (EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : descriptor.rigidBodyMappings) {
			rigidBodyMapping.entity = stream.read<Entity>();
			rigidBodyMapping.component.position = stream.read<svec2<float32>>();
			rigidBodyMapping.component.rotation = stream.read<float32>();
			rigidBodyMapping.component.scale = stream.read<svec2<float32>>();
			rigidBodyMapping.component.linearVelocity = stream.read<svec2<float32>>();
			rigidBodyMapping.component.angularVelocity = stream.read<float32>();
			rigidBodyMapping.component.inverseMass = stream.read<float32>();
			rigidBodyMapping.component.inverseInertia = stream.read<float32>();
			rigidBodyMapping.component.friction = stream.read<float32>();
			rigidBodyMapping.component.restitution = stream.read<float32>();
		}

		return createRef<Scene2DAsset>(descriptor);
	}
}