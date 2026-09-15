// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/File.hpp"

namespace Trident {
	template<>
	struct File<FileFormat::FBX>;

	namespace FBX {
		struct Node {
			sequence<char8> name;
			range<uint64, uint64> propertyRange;
			range<uint64, uint64> nodeRange;
		};

		enum class PropertyTypeCode : char8 {
			/// 2-byte signed integer.
			Y = 'Y',

			/// 1-bit boolean (encoded as the LSB of a 1èbyte value).
			C = 'C',

			/// 4-byte signed integer.
			I = 'I',

			/// 4-byte single-precision IEEE 754 number.
			F = 'F',

			/// 8-byte double-precision IEEE 754 number.
			D = 'D',

			/// 8-byte signed integer.
			L = 'L',

			/// Array of 4-byte single-precision IEEE 754 numbers.
			f = 'f',

			/// Array of 8-byte double-precision IEEE 754 numbers.
			d = 'd',

			/// Array of 8-byte signed integers.
			l = 'l',

			/// Array of 4-byte signed integers.
			i = 'i',

			/// Array of 1-byte booleans.
			b = 'b',

			/// String.
			S = 'S',

			/// Raw binary data.
			R = 'R',
		};

		struct Property {
			PropertyTypeCode typeCode;
		};

		template<typename DataType>
		struct PrimitiveProperty : public Property {
			DataType data;
		};

		template struct PrimitiveProperty<bool>;
		template struct PrimitiveProperty<sint16>;
		template struct PrimitiveProperty<sint32>;
		template struct PrimitiveProperty<float32>;
		template struct PrimitiveProperty<sint64>;
		template struct PrimitiveProperty<float64>;

		template<typename DataType>
		struct ArrayProperty : public Property {
			sequence<DataType> contents;
		};

		template struct ArrayProperty<bool>;
		template struct ArrayProperty<sint32>;
		template struct ArrayProperty<float32>;
		template struct ArrayProperty<sint64>;
		template struct ArrayProperty<float64>;

		struct SpecialProperty : public Property {
			sequence<uint8> data;
		};

		/// Recursively jumps over the node and its children.
		/// Returns the node's end offset.
		uint64 countNodesAndProperties(ByteStream<StreamOperation::READ>& stream, uint32 fileVersion, std::vector<uint64>& childNodeCounts, uint64& propertyCount);

		/// Recursively parses the node and its children and writes the decoded data in the file.
		/// Returns the node's end offset.
		uint64 readNode(ByteStream<StreamOperation::READ>& stream, File<FileFormat::FBX>& file, uint64 nodeOffset, uint64& childNodeOffset, const uint64*& childNodeCount, uint64& propertyOffset);

		ref<FBX::Property> readProperty(ByteStream<StreamOperation::READ>& stream);

		#if defined(BUILD_MODE_DEBUG)
			void debugFile(const File<FileFormat::FBX>& file, ConsoleLogger& logger);

			void debugNode(const File<FileFormat::FBX>& file, const Node* node, ConsoleLogger& logger, uint8 indentSize = 0);

			void debugProperty(ref<Property> property, ConsoleLogger& logger, uint8 indentSize = 0);
		#endif

		const Node* findNode(const File<FileFormat::FBX>& file, view<char8> name);
		const Node* findNode(const File<FileFormat::FBX>& file, const Node* parentNode, view<char8> name);

		/// `node` must be a 'Properties70' node.
		template<typename DataType>
		DataType resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);

		template bool resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);
		template sint32 resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);
		template float32 resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);
		template sint64 resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);
		template float64 resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* node, view<char8> name);
	}

	template<>
	struct File<FileFormat::FBX> {
		uint32 version;
		sequence<FBX::Node> nodes;
		uint64 rootNodeCount;
		sequence<ref<FBX::Property>> properties;
	};

	template<>
	bool Decoder<FileFormat::FBX>::validateSignature(const ByteStream<StreamOperation::READ>& stream);

	/// References:
	/// - https://code.blender.org/2013/08/fbx-binary-file-format-specification
	/// - https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format
	template<>
	File<FileFormat::FBX> Decoder<FileFormat::FBX>::decode(view<uint8> data);
}