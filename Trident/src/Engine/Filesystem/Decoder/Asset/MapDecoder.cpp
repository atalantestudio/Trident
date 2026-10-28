// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/API/Map/Map.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	Map Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Map map{};
		map.identifier = stream.read<uint32>();
		map.version = stream.read<VersionNumber>();

		// skyColor
		stream.jump(sizeof(vec3<float32>));

		// skyboxAssetIdentifier
		stream.jump(sizeof(uint32));

		const uint8 indexByteSize = stream.read<uint8>();

		ENGINE_ASSERT(indexByteSize == 4, "Only 32-bit indices are supported.");

		// vertexCount
		stream.jump(sizeof(uint32));

		// staticVertexCount
		// TODO(scene/3d): Remove in the asset.
		stream.jump(sizeof(uint32));

		const uint32 bakedVertexCount = stream.read<uint32>();

		// triangleCount
		stream.jump(sizeof(uint32));

		const uint32 bakedTriangleCount = stream.read<uint32>();
		const uint32 bakedIndexCount = bakedTriangleCount * 3;

		// faceLineIndexCount
		const uint32 faceLineIndexCount = stream.read<uint32>();

		// lightmapCoordinateCount
		const uint32 lightmapCoordinateCount = stream.read<uint32>();

		map.staticGeometry.vertices = sequence<Vertex3D>(bakedVertexCount);
		map.staticGeometry.indices = sequence<uint32>(bakedIndexCount);

		// staticInstanceCount
		stream.jump(sizeof(uint32));

		// geometryCount
		stream.jump(sizeof(uint32));

		// staticMaterialShaderCount
		stream.jump(sizeof(uint32));

		// dynamicMaterialShaderCount
		stream.jump(sizeof(uint32));

		// materialGeometryCount
		stream.jump(sizeof(uint32));

		// materialCount
		stream.jump(sizeof(uint32));

		// entityCount
		stream.jump(sizeof(uint32));

		// sunDirection
		stream.jump(sizeof(vec3<float32>));

		// sunColor
		stream.jump(sizeof(vec3<float32>));

		// pointLightCount
		stream.jump(sizeof(uint32));

		// lightmapAssetIdentifier
		stream.jump(sizeof(uint32));

		map.playerSpawn.position = stream.read<vec3<float32>>();
		map.playerSpawn.viewDirection = stream.read<vec3<float32>>();

		std::copy_n(pointer_cast<const Vertex3D*>(stream.getBytePosition()), bakedVertexCount, &map.staticGeometry.vertices[0]);

		stream.jump(bakedVertexCount * sizeof(Vertex3D));

		std::copy_n(pointer_cast<const uint32*>(stream.getBytePosition()), bakedIndexCount, &map.staticGeometry.indices[0]);

		stream.jump(bakedIndexCount * sizeof(uint32));

		if (faceLineIndexCount > 0) {
			stream.jump(faceLineIndexCount * sizeof(uint32));
		}

		if (lightmapCoordinateCount > 0) {
			stream.jump(lightmapCoordinateCount * sizeof(uint32));
		}

		/* while (asset.entityInstanceOffset < staticInstanceCount) {
			const vec3<float32> position = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			const vec3<float32> rotation = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			const float32 scale = stream.read<float32>();
			const quat<float32> orientation = normalized(
				rotationAxisAngle<quat<float32>>(UP, rotation[1]) *
				rotationAxisAngle<quat<float32>>(RIGHT, rotation[0]) *
				rotationAxisAngle<quat<float32>>(FORWARD, rotation[2])
			);

			Instance3D& instance = asset.instances[asset.entityInstanceOffset];
			instance.worldTransform = (
				uniformScaling<mat4x4<float32>>(scale) *
				mat4x4<float32>(orientation) *
				translation<mat4x4<float32>>(position)
			);

			instance.normalTransform = transpose(inverse(mat3x3<float32>{
				instance.worldTransform[0][0], instance.worldTransform[0][1], instance.worldTransform[0][2],
				instance.worldTransform[1][0], instance.worldTransform[1][1], instance.worldTransform[1][2],
				instance.worldTransform[2][0], instance.worldTransform[2][1], instance.worldTransform[2][2]
			}));

			asset.entityInstanceOffset += 1;
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
			geometryIndex += 1;
		} */

		return map;
	}
}