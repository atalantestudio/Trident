// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	template<>
	ref<GeometryAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream) {
		Descriptor<GeometryAsset> descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();
		descriptor.flags = static_cast<GeometryAssetFlags>(stream.read<uint32>());

		if (descriptor.flags & GeometryAssetFlags::IS_2D) {
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

			return createRef<GeometryAsset>(descriptor);
		}

		if (descriptor.flags & GeometryAssetFlags::IS_3D) {
			if (descriptor.flags & GeometryAssetFlags::HAS_HULL) {
				ENGINE_ERROR_UNIMPLEMENTED();
			}

			const uint32 vertexPositionCount = stream.read<uint32>();

			descriptor.vertexPositionComponents = sequence<float32>(vertexPositionCount * Geometry3D::VERTEX_POSITION_STRIDE);

			for (float32& vertexPositionComponent : descriptor.vertexPositionComponents) {
				vertexPositionComponent = stream.read<float32>();
			}

			const uint32 vertexNormalCount = stream.read<uint32>();

			descriptor.vertexNormalComponents = sequence<float32>(vertexNormalCount * Geometry3D::VERTEX_NORMAL_STRIDE);

			for (float32& vertexNormalComponent : descriptor.vertexNormalComponents) {
				vertexNormalComponent = stream.read<float32>();
			}

			const uint32 vertexTextureCoordinateCount = stream.read<uint32>();

			descriptor.vertexTextureCoordinateComponents = sequence<float32>(vertexTextureCoordinateCount * Geometry3D::VERTEX_TEXTURE_COORDINATE_STRIDE);

			for (float32& vertexTextureCoordinateComponent : descriptor.vertexTextureCoordinateComponents) {
				vertexTextureCoordinateComponent = stream.read<float32>();
			}

			const uint32 largestVertexPositionIndex = vertexPositionCount - 1;
			const uint32 largestVertexNormalIndex = vertexNormalCount - 1;
			const uint32 largestVertexTextureCoordinateIndex = vertexTextureCoordinateCount - 1;

			const uint8 vertexPositionIndexBitCount = countBits(largestVertexPositionIndex);
			const uint8 vertexNormalIndexBitCount = countBits(largestVertexNormalIndex);
			const uint8 vertexTextureCoordinateIndexBitCount = countBits(largestVertexTextureCoordinateIndex);

			const uint32 indexCount = stream.read<uint32>();

			descriptor.vertexPositionIndices = sequence<uint32>(indexCount);
			descriptor.vertexTextureCoordinateIndices = sequence<uint32>(indexCount);
			descriptor.vertexNormalIndices = sequence<uint32>(indexCount);

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

			return createRef<GeometryAsset>(descriptor);
		}

		ENGINE_ASSERT(false, "Invalid geometry asset data.");

		return nullptr;
	}
}