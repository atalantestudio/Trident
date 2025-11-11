#include "pch.hpp"

#include "Engine/Filesystem/File/FBXFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	static constexpr uint32 MAX_ROOT_NODE_ITERATIONS = 11;
	static constexpr uint32 MAX_CHILD_NODE_ITERATIONS = 11002;

	template<typename DataType>
	static ref<FBXProperty> readPrimitiveProperty(ByteStream<StreamOperation::READ>& stream) {
		ref<FBXPrimitiveProperty<DataType>> Property = createRef<FBXPrimitiveProperty<DataType>>();
		Property->Data = stream.read<DataType>();

		return Property;
	}

	template<typename DataType>
	static ref<FBXProperty> readArrayProperty(ByteStream<StreamOperation::READ>& stream) {
		const uint32 ArrayLength = stream.read<uint32>();
		const uint32 Encoding = stream.read<uint32>();
		const bool Compressed = static_cast<bool>(Encoding);
		const uint32 CompressedLength = stream.read<uint32>();

		ref<FBXArrayProperty<DataType>> Property = createRef<FBXArrayProperty<DataType>>();

		if (Compressed) {
			BitStream<StreamOperation::READ> bitStream(stream.getBytePosition(), CompressedLength, Endianness::LITTLE_ENDIAN);

			Property->Contents = bitStream.inflate<DataType>(ArrayLength);

			stream.jump(CompressedLength);
		}
		else {
			Property->Contents = sequence<DataType>(ArrayLength);

			std::copy_n(stream.getBytePosition(), ArrayLength, Property->Contents.begin());

			stream.jump(ArrayLength * sizeof(DataType));
		}

		return Property;
	}

	static ref<FBXProperty> readSpecialProperty(ByteStream<StreamOperation::READ>& stream) {
		const uint32 Length = stream.read<uint32>();

		ref<FBXSpecialProperty> Property = createRef<FBXSpecialProperty>();
		Property->Data = sequence<uint8>(Length);

		// TODO: Add null terminator for the string case?
		std::copy_n(stream.getBytePosition(), Length, Property->Data.begin());

		stream.jump(Property->Data.count());

		return Property;
	}

	static inline const std::unordered_map<FBXPropertyTypeCode, std::function<ref<FBXProperty>(ByteStream<StreamOperation::READ>&)>> readProperty = {
		{FBXPropertyTypeCode::Y, readPrimitiveProperty<sint16>},
		{FBXPropertyTypeCode::C, readPrimitiveProperty<bool>},
		{FBXPropertyTypeCode::I, readPrimitiveProperty<sint32>},
		{FBXPropertyTypeCode::F, readPrimitiveProperty<float32>},
		{FBXPropertyTypeCode::D, readPrimitiveProperty<float64>},
		{FBXPropertyTypeCode::L, readPrimitiveProperty<sint64>},
		{FBXPropertyTypeCode::f, readArrayProperty<float32>},
		{FBXPropertyTypeCode::d, readArrayProperty<float64>},
		{FBXPropertyTypeCode::l, readArrayProperty<sint64>},
		{FBXPropertyTypeCode::i, readArrayProperty<sint32>},
		{FBXPropertyTypeCode::b, readArrayProperty<bool>},
		{FBXPropertyTypeCode::S, readSpecialProperty},
		{FBXPropertyTypeCode::R, readSpecialProperty},
	};

	static FBXNode readNode(ByteStream<StreamOperation::READ>& stream, bool pre7500) {
		FBXNode Node;
		uint64 NumProperties;

		if (pre7500) {
			Node.EndOffset = stream.read<uint32>();
			NumProperties = stream.read<uint32>();

			stream.jump(sizeof(uint32));
		}
		else {
			Node.EndOffset = stream.read<uint64>();
			NumProperties = stream.read<uint64>();

			stream.jump(sizeof(uint64));
		}

		const uint8 NameLen = stream.read<uint8>();

		// Skip null node.
		if (Node.EndOffset == 0) {
			return Node;
		}

		Node.Name = sequence<char8>(NameLen + 1);
		Node.Name[NameLen] = '\0';

		std::copy_n(stream.getBytePosition(), NameLen, Node.Name.begin());

		stream.jump(NameLen);

		Node.PropertyList = sequence<ref<FBXProperty>>(NumProperties);

		for (uint32 propertyIndex = 0; propertyIndex < NumProperties; propertyIndex += 1) {
			const FBXPropertyTypeCode TypeCode = stream.read<FBXPropertyTypeCode>();

			Node.PropertyList[propertyIndex] = readProperty.at(TypeCode)(stream);
			Node.PropertyList[propertyIndex]->TypeCode = TypeCode;
		}

		if (stream.getByteOffset() == Node.EndOffset) {
			return Node;
		}

		for (uint32 childNodeIndex = 0; childNodeIndex < MAX_CHILD_NODE_ITERATIONS; childNodeIndex += 1) {
			const FBXNode ChildNode = readNode(stream, pre7500);

			if (ChildNode.EndOffset == 0) {
				break;
			}

			Node.NestedList.emplace_back(ChildNode);
		}

		if (Node.NestedList.size() >= MAX_CHILD_NODE_ITERATIONS) {
			ENGINE_WARNING("Recursion limit reached for node \"[]\" ([]).", &Node.Name[0], MAX_CHILD_NODE_ITERATIONS);
		}

		return Node;
	}

	template<>
	bool Filesystem::validateSignature<FileFormat::FBX>(const ByteStream<StreamOperation::READ>& stream) {
		if (stream.getRemainingSize() < 22) {
			return false;
		}

		const char8* magic = pointer_cast<const char8*>(stream.getBytePosition());

		return std::strcmp(magic, "Kaydara FBX Binary  \x00") == 0;
	}

	// https://code.blender.org/2013/08/fbx-binary-file-format-specification
	// https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format
	template<>
	File<FileFormat::FBX> Filesystem::decode(const sequence<uint8>& data) {
		File<FileFormat::FBX> file;

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(Filesystem::validateSignature<FileFormat::FBX>(stream), "Invalid FBX file signature.");

		stream.jump(21);

		// Skip bytes [0x1a, 0x00].
		// NOTE: According to https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure, they are unknown but do appear in all observed files.
		stream.jump(2);

		file.Version = stream.read<uint32>();

		for (uint32 rootNodeIndex = 0; rootNodeIndex < MAX_ROOT_NODE_ITERATIONS; rootNodeIndex += 1) {
			const FBXNode Node = readNode(stream, file.Version < 7500);

			if (Node.EndOffset == 0) {
				break;
			}

			file.NodeList.emplace_back(Node);
		}

		return file;
	}
}