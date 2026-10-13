// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/Scene2DAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	Scene2DAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Scene2DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();

		const uint32 staticTriangleCount = stream.read<uint32>();
		const uint32 staticVertexCount = stream.read<uint32>();
		const uint32 staticTextureCoordinateCount = stream.read<uint32>();

		asset.dynamicVertexPositionCount = 0;
		asset.dynamicVertexTextureCoordinateCount = 0;
		asset.dynamicVertexPositionIndexCount = 0;
		asset.dynamicVertexTextureCoordinateIndexCount = 0;
		asset.entityCount = stream.read<uint32>();
		asset.hullCount = 0;
		asset.staticTriangles = sequence<StaticTriangle2D>(staticTriangleCount);
		asset.staticVertices = sequence<vec2<float32>>(staticVertexCount);
		asset.staticTextureCoordinates = sequence<vec2<float32>>(staticTextureCoordinateCount);

		const uint32 geometryAssetCount = stream.read<uint32>();
		const uint32 geometryMappingCount = stream.read<uint32>();
		const uint32 rigidBodyMappingCount = stream.read<uint32>();

		asset.geometryAssets = sequence<ref<Geometry2DAsset>>(geometryAssetCount);
		asset.geometryMappings = sequence<EntityComponentMapping<Geometry2DComponent>>(geometryMappingCount);
		asset.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody2DComponent>>(rigidBodyMappingCount);

		for (StaticTriangle2D& triangle : asset.staticTriangles) {
			triangle.aIndex = stream.read<uint32>();
			triangle.bIndex = stream.read<uint32>();
			triangle.cIndex = stream.read<uint32>();
			triangle.aUvIndex = stream.read<uint32>();
			triangle.bUvIndex = stream.read<uint32>();
			triangle.cUvIndex = stream.read<uint32>();
		}

		for (vec2<float32>& vertex : asset.staticVertices) {
			vertex = stream.read<vec2<float32>>();
		}

		for (vec2<float32>& textureCoordinate : asset.staticTextureCoordinates) {
			textureCoordinate = stream.read<vec2<float32>>();
		}

		for (ref<Geometry2DAsset>& geometryAsset : asset.geometryAssets) {
			const uint32 identifier = stream.read<uint32>();

			geometryAsset = assetLibrary->getAsset<Geometry2DAsset>(identifier);

			asset.dynamicVertexPositionCount += geometryAsset->getPositionCount();
			asset.dynamicVertexTextureCoordinateCount += geometryAsset->getTextureCoordinateCount();
			asset.dynamicVertexPositionIndexCount += geometryAsset->vertexPositionIndices.count();

			if (geometryAsset->flags & GeometryAssetFlags::HAS_HULL) {
				// TODO: Don't mix triangle and line indices?
				asset.dynamicVertexPositionIndexCount += geometryAsset->hullLineIndices.count();

				asset.hullCount += 1;
			}
		}

		for (EntityComponentMapping<Geometry2DComponent>& geometryMapping : asset.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component = stream.read<Geometry2DComponent>();
		}

		for (EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : asset.rigidBodyMappings) {
			rigidBodyMapping.entity = stream.read<Entity>();
			rigidBodyMapping.component.position = stream.read<vec2<float32>>();
			rigidBodyMapping.component.rotation = stream.read<float32>();
			rigidBodyMapping.component.scale = stream.read<vec2<float32>>();
			rigidBodyMapping.component.linearVelocity = stream.read<vec2<float32>>();
			rigidBodyMapping.component.angularVelocity = stream.read<float32>();
			rigidBodyMapping.component.inverseMass = stream.read<float32>();
			rigidBodyMapping.component.inverseInertia = stream.read<float32>();
			rigidBodyMapping.component.friction = stream.read<float32>();
			rigidBodyMapping.component.restitution = stream.read<float32>();
		}

		return asset;
	}
}