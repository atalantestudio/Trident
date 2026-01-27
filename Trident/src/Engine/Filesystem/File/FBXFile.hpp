#pragma once

#include "Engine/Filesystem/File.hpp"

namespace Trident {
	enum class FBXPropertyTypeCode : char8 {
		// 2-byte signed integer.
		Y = 'Y',
		// 1-bit boolean (encoded as the LSB of a 1èbyte value).
		C = 'C',
		// 4-byte signed integer.
		I = 'I',
		// 4-byte single-precision IEEE 754 number.
		F = 'F',
		// 8-byte double-precision IEEE 754 number.
		D = 'D',
		// 8-byte signed integer.
		L = 'L',
		// Array of 4-byte single-precision IEEE 754 numbers.
		f = 'f',
		// Array of 8-byte double-precision IEEE 754 numbers.
		d = 'd',
		// Array of 8-byte signed integers.
		l = 'l',
		// Array of 4-byte signed integers.
		i = 'i',
		// Array of 1-byte booleans.
		b = 'b',
		// String.
		S = 'S',
		// Raw binary data.
		R = 'R',
	};

	struct FBXProperty {
		FBXPropertyTypeCode TypeCode;
	};

	template<typename DataType>
	struct FBXPrimitiveProperty : public FBXProperty {
		DataType Data;
	};

	template struct FBXPrimitiveProperty<sint16>;
	template struct FBXPrimitiveProperty<bool>;
	template struct FBXPrimitiveProperty<sint32>;
	template struct FBXPrimitiveProperty<float32>;
	template struct FBXPrimitiveProperty<float64>;
	template struct FBXPrimitiveProperty<sint64>;

	template<typename DataType>
	struct FBXArrayProperty : public FBXProperty {
		sequence<DataType> Contents;
	};

	template struct FBXArrayProperty<float32>;
	template struct FBXArrayProperty<float64>;
	template struct FBXArrayProperty<sint64>;
	template struct FBXArrayProperty<sint32>;
	template struct FBXArrayProperty<bool>;

	struct FBXSpecialProperty : public FBXProperty {
		sequence<uint8> Data;
	};

	struct FBXNode {
		uint64 EndOffset;
		sequence<char8> Name;
		sequence<ref<FBXProperty>> PropertyList;
		std::vector<FBXNode> NestedList;
	};

	template<>
	class File<FileFormat::FBX> {
		public:
			static const FBXNode* searchNodeRecursive(const std::vector<FBXNode>& NestedList, view<char8> Name, const FBXNode* ResultNode = nullptr) {
				for (const FBXNode& Node : NestedList) {
					if (Name == Node.Name) {
						ResultNode = &Node;

						break;
					}

					ResultNode = searchNodeRecursive(Node.NestedList, Name, ResultNode);
				}

				return ResultNode;
			}

		public:
			uint32 Version;
			std::vector<FBXNode> NodeList;
	};
}