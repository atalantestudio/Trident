// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

#include "../File/FBX.hpp"

namespace Trident {
	namespace FBX {
		template<typename DataType>
		static ref<Property> readPrimitiveProperty(ByteStream<StreamOperation::READ>& stream) {
			ref<FBX::PrimitiveProperty<DataType>> property = allocateRef<FBX::PrimitiveProperty<DataType>>();
			property->data = stream.read<DataType>();

			return property;
		}

		template<typename DataType>
		static ref<Property> readArrayProperty(ByteStream<StreamOperation::READ>& stream) {
			const uint32 arrayLength = stream.read<uint32>();
			const uint32 encoding = stream.read<uint32>();
			const bool compressed = static_cast<bool>(encoding);
			const uint32 compressedLength = stream.read<uint32>();

			ref<FBX::ArrayProperty<DataType>> property = allocateRef<FBX::ArrayProperty<DataType>>();

			if (compressed) {
				BitStream<StreamOperation::READ> bitStream(stream.getBytePosition(), compressedLength, Endianness::LITTLE_ENDIAN);

				property->contents = bitStream.inflate<DataType>(arrayLength);

				stream.jump(compressedLength);
			}
			else {
				property->contents = sequence<DataType>(arrayLength);

				std::copy_n(stream.getBytePosition(), arrayLength, &property->contents[0]);

				stream.jump(arrayLength * sizeof(DataType));
			}

			return property;
		}

		static ref<Property> readSpecialProperty(ByteStream<StreamOperation::READ>& stream) {
			const uint32 length = stream.read<uint32>();

			ref<FBX::SpecialProperty> property = allocateRef<FBX::SpecialProperty>();
			property->data = sequence<uint8>(length);

			std::copy_n(stream.getBytePosition(), length, &property->data[0]);

			stream.jump(property->data.count());

			return property;
		}

		inline static const std::unordered_map<PropertyTypeCode, std::function<ref<Property>(ByteStream<StreamOperation::READ>&)>> readPropertyMap = {
			{PropertyTypeCode::Y, readPrimitiveProperty<sint16>},
			{PropertyTypeCode::C, readPrimitiveProperty<bool>},
			{PropertyTypeCode::I, readPrimitiveProperty<sint32>},
			{PropertyTypeCode::F, readPrimitiveProperty<float32>},
			{PropertyTypeCode::D, readPrimitiveProperty<float64>},
			{PropertyTypeCode::L, readPrimitiveProperty<sint64>},
			{PropertyTypeCode::f, readArrayProperty<float32>},
			{PropertyTypeCode::d, readArrayProperty<float64>},
			{PropertyTypeCode::l, readArrayProperty<sint64>},
			{PropertyTypeCode::i, readArrayProperty<sint32>},
			{PropertyTypeCode::b, readArrayProperty<bool>},
			{PropertyTypeCode::S, readSpecialProperty},
			{PropertyTypeCode::R, readSpecialProperty},
		};
	}

	uint64 FBX::countNodesAndProperties(ByteStream<StreamOperation::READ>& stream, uint32 fileVersion, std::vector<uint64>& childNodeCounts, uint64& propertyCount) {
		uint64 endOffset;
		uint64 propertyByteSize;

		if (fileVersion >= 7500) {
			endOffset = stream.read<uint64>();
			propertyCount += stream.read<uint64>();
			propertyByteSize = stream.read<uint64>();
		}
		else {
			endOffset = stream.read<uint32>();
			propertyCount += stream.read<uint32>();
			propertyByteSize = stream.read<uint32>();
		}

		const uint8 nameByteSize = stream.read<uint8>();

		if (endOffset == 0) {
			return endOffset;
		}

		stream.jump(nameByteSize + propertyByteSize);

		const uint64 nodeOffset = childNodeCounts.size();

		childNodeCounts.emplace_back(0);

		while (stream.getByteOffset() < endOffset) {
			const uint64 nodeEndOffset = countNodesAndProperties(stream, fileVersion, childNodeCounts, propertyCount);

			if (nodeEndOffset == 0) {
				break;
			}

			childNodeCounts[nodeOffset] += 1;
		}

		return endOffset;
	}

	uint64 FBX::readNode(ByteStream<StreamOperation::READ>& stream, File<FileFormat::FBX>& file, uint64 nodeOffset, uint64& childNodeOffset, const uint64*& childNodeCount, uint64& propertyOffset) {
		uint64 endOffset;
		uint64 propertyCount;

		if (file.version >= 7500) {
			endOffset = stream.read<uint64>();
			propertyCount = stream.read<uint64>();

			stream.jump(sizeof(uint64));
		}
		else {
			endOffset = stream.read<uint32>();
			propertyCount = stream.read<uint32>();

			stream.jump(sizeof(uint32));
		}

		const uint8 nameByteSize = stream.read<uint8>();

		if (endOffset == 0) {
			return endOffset;
		}

		Node& node = file.nodes[nodeOffset];
		node.name = sequence<char8>(pointer_cast<const char8*>(stream.getBytePosition()), nameByteSize);
		node.propertyRange.offset = propertyOffset;
		node.propertyRange.count = propertyCount;
		node.nodeRange.offset = childNodeOffset;
		node.nodeRange.count = *childNodeCount;

		childNodeOffset += node.nodeRange.count;
		childNodeCount += 1;

		stream.jump(nameByteSize);

		uint64 propertyIndex = 0;

		while (propertyIndex < propertyCount) {
			file.properties[propertyOffset + propertyIndex] = readProperty(stream);

			propertyIndex += 1;
		}

		propertyOffset += propertyCount;

		uint64 nodeIndex = 0;

		while (stream.getByteOffset() < endOffset) {
			const uint64 nodeEndOffset = readNode(stream, file, node.nodeRange.offset + nodeIndex, childNodeOffset, childNodeCount, propertyOffset);

			if (nodeEndOffset == 0) {
				break;
			}

			nodeIndex += 1;
		}

		return endOffset;
	}

	ref<FBX::Property> FBX::readProperty(ByteStream<StreamOperation::READ>& stream) {
		const PropertyTypeCode typeCode = stream.read<PropertyTypeCode>();

		ref<Property> property = readPropertyMap.at(typeCode)(stream);
		property->typeCode = typeCode;

		return property;
	}

	template<>
	bool Decoder<FileFormat::FBX>::validateSignature(const ByteStream<StreamOperation::READ>& stream) {
		static constexpr view<char8> MAGIC("Kaydara FBX Binary  \x00", 22);

		if (stream.getRemainingSize() < 22) {
			return false;
		}

		const view<char8> magic(pointer_cast<const char8*>(stream.getBytePosition()), 22);

		return magic == MAGIC;
	}

	template<>
	File<FileFormat::FBX> Decoder<FileFormat::FBX>::decode(view<uint8> data) {
		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(Decoder<FileFormat::FBX>::validateSignature(stream), "Invalid FBX file signature.");

		/// The last 2 bytes [0x1a, 0x00] are unknown but appear in all observed files.
		/// See https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure.
		stream.jump(23);

		std::vector<uint64> childNodeCounts;
		uint64 propertyCount = 0;
		uint64 rootNodeCount = 0;

		File<FileFormat::FBX> file{};
		file.version = stream.read<uint32>();

		const uint64 firstNodeOffset = stream.getByteOffset();

		while (true) {
			const uint64 endOffset = FBX::countNodesAndProperties(stream, file.version, childNodeCounts, propertyCount);

			if (endOffset == 0) {
				break;
			}

			rootNodeCount += 1;
		}

		file.nodes = sequence<FBX::Node>(childNodeCounts.size());
		file.properties = sequence<ref<FBX::Property>>(propertyCount);
		file.rootNodeCount = rootNodeCount;

		stream.seek(firstNodeOffset);

		const uint64* childNodeCount = &childNodeCounts[0];

		uint64 childNodeOffset = file.rootNodeCount;

		propertyCount = 0;
		rootNodeCount = 0;

		while (rootNodeCount < file.rootNodeCount) {
			FBX::readNode(stream, file, rootNodeCount, childNodeOffset, childNodeCount, propertyCount);

			rootNodeCount += 1;
		}

		return file;
	}
}