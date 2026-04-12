// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Geometry3D.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	static void decodeGeometryAsset2D(ByteStream<StreamOperation::READ>& stream, Descriptor<GeometryAsset>& descriptor) {
		const uint32 vertexCount = stream.read<uint32>();
		const uint32 indexCount = stream.read<uint32>();

		descriptor.vertexPositionComponents = sequence<float32>(vertexCount * 2);
		descriptor.vertexTextureCoordinateComponents = sequence<float32>(vertexCount * 2);
		descriptor.vertexPositionIndices = sequence<uint32>(indexCount);

		std::copy_n(pointer_cast<const float32*>(stream.getBytePosition()), descriptor.vertexPositionComponents.count(), &descriptor.vertexPositionComponents[0]);

		stream.jump(descriptor.vertexPositionComponents.count() * sizeof(float32));

		std::copy_n(pointer_cast<const float32*>(stream.getBytePosition()), descriptor.vertexTextureCoordinateComponents.count(), &descriptor.vertexTextureCoordinateComponents[0]);

		stream.jump(descriptor.vertexTextureCoordinateComponents.count() * sizeof(float32));

		std::copy_n(pointer_cast<const uint32*>(stream.getBytePosition()), descriptor.vertexPositionIndices.count(), &descriptor.vertexPositionIndices[0]);

		stream.jump(descriptor.vertexPositionIndices.count() * sizeof(uint32));

		if (descriptor.flags & GeometryAssetFlags::HAS_HULL) {
			descriptor.hullType = static_cast<Hull2DType>(stream.read<uint32>());

			stream.jump(sizeof(uint32));

			const svec2<float32> centroid = stream.read<svec2<float32>>();

			descriptor.hullCentroid = {centroid[0], centroid[1], 0.0f};
			descriptor.hullRadius = stream.read<float32>();
			descriptor.hullLineIndices = sequence<uint32>(stream.read<uint32>());

			std::copy_n(pointer_cast<const uint32*>(stream.getBytePosition()), descriptor.hullLineIndices.count(), &descriptor.hullLineIndices[0]);

			stream.jump(descriptor.hullLineIndices.count() * sizeof(uint32));
		}
	}

	static void decodeGeometryAsset3D(ByteStream<StreamOperation::READ>& stream, Descriptor<GeometryAsset>& descriptor) {
		const uint32 vertexPositionCount = stream.read<uint32>();
		const uint32 vertexNormalCount = stream.read<uint32>();
		const uint32 vertexTextureCoordinateCount = stream.read<uint32>();
		const uint32 triangleTangentCount = stream.read<uint32>();
		const uint32 indexCount = stream.read<uint32>();

		descriptor.vertexPositionComponents = sequence<float32>(vertexPositionCount * 3);
		descriptor.vertexNormalComponents = sequence<float32>(vertexNormalCount * 3);
		descriptor.vertexTextureCoordinateComponents = sequence<float32>(vertexTextureCoordinateCount * 2);
		descriptor.triangleTangentComponents = sequence<float32>(triangleTangentCount * 3);
		descriptor.vertexPositionIndices = sequence<uint32>(indexCount);
		descriptor.vertexTextureCoordinateIndices = sequence<uint32>(indexCount);
		descriptor.vertexNormalIndices = sequence<uint32>(indexCount);

		for (float32& vertexPositionComponent : descriptor.vertexPositionComponents) {
			vertexPositionComponent = stream.read<float32>();
		}

		for (float32& vertexNormalComponent : descriptor.vertexNormalComponents) {
			vertexNormalComponent = stream.read<float32>();
		}

		for (float32& vertexTextureCoordinateComponent : descriptor.vertexTextureCoordinateComponents) {
			vertexTextureCoordinateComponent = stream.read<float32>();
		}

		for (float32& triangleTangentComponent : descriptor.triangleTangentComponents) {
			triangleTangentComponent = stream.read<float32>();
		}

		const uint8 vertexPositionIndexBitCount = countBits(vertexPositionCount - 1);
		const uint8 vertexNormalIndexBitCount = countBits(vertexNormalCount - 1);
		const uint8 vertexTextureCoordinateIndexBitCount = countBits(vertexTextureCoordinateCount - 1);

		BitStream<StreamOperation::READ> bitStream(stream.getBytePosition(), stream.getRemainingSize(), Endianness::LITTLE_ENDIAN);

		uint32 indexOffset = 0;

		while (indexOffset < indexCount) {
			descriptor.vertexPositionIndices[indexOffset] = bitStream.read<uint32>(vertexPositionIndexBitCount);

			indexOffset += 1;
		}

		indexOffset = 0;

		while (indexOffset < indexCount) {
			descriptor.vertexNormalIndices[indexOffset] = bitStream.read<uint32>(vertexNormalIndexBitCount);

			indexOffset += 1;
		}

		indexOffset = 0;

		while (indexOffset < indexCount) {
			descriptor.vertexTextureCoordinateIndices[indexOffset] = bitStream.read<uint32>(vertexTextureCoordinateIndexBitCount);

			indexOffset += 1;
		}
	}

	template<>
	ref<GeometryAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream) {
		Descriptor<GeometryAsset> descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();
		descriptor.flags = stream.read<GeometryAssetFlags>();

		stream.jump(sizeof(uint8) * 3);

		if (descriptor.flags & GeometryAssetFlags::IS_2D) {
			decodeGeometryAsset2D(stream, descriptor);

			return createRef<GeometryAsset>(descriptor);
		}

		if (descriptor.flags & GeometryAssetFlags::IS_3D) {
			decodeGeometryAsset3D(stream, descriptor);

			return createRef<GeometryAsset>(descriptor);
		}

		ENGINE_ASSERT(false, "Invalid geometry asset data.");

		return nullptr;
	}
}