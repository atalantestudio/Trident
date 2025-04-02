#include "pch.hpp"

namespace ProjectA {
	// https://code.blender.org/2013/08/fbx-binary-file-format-specification
	// https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format
	enum class FBXPropertyTypeCode : char {
		// 2 byte signed integer.
		Y = 'Y',
		// 1 bit boolean (encoded as the LSB of a 1 byte value).
		C = 'C',
		// 4 byte signed integer.
		I = 'I',
		// 4 byte single-precision IEEE 754 number.
		F = 'F',
		// 8 byte double-precision IEEE 754 number.
		D = 'D',
		// 8 byte signed integer.
		L = 'L',
		// Array of 4 byte single-precision IEEE 754 numbers.
		f = 'f',
		// Array of 8 byte double-precision IEEE 754 numbers.
		d = 'd',
		// Array of 8 byte signed integers.
		l = 'l',
		// Array of 4 byte signed integers.
		i = 'i',
		// Array of 1 byte booleans.
		b = 'b',
		// String.
		S = 'S',
		// Raw binary data.
		R = 'R',
	};
}

/*
- Read header
	- Check magic
	- Read Version
- Until MAX_ROOT_NODE_ITERATIONS or Node.EndOffset == 0
	- Read EndOffset
	- Read NumProperties
	- Read PropertyListLen
	- Read NameLen
	- Read Name(NameLen)
	- For each node property
		- Read TypeCode
		- If TypeCode is primitive
			- Read Data as T
		- If TypeCode is array
			- Read ArrayLength
			- Read Encoding
			- Read CompressedLength
			- If Encoding == 0
				- Read Contents(ArrayLength)
			- If Encoding == 1
				- [TODO] Decode Contents(CompressedLength) in DecodedContents
				- Read Contents = DecodedContents
		- If TypeCode is S
			- Read Length
			- Read Data(Length) as char
			- Add null terminator
		- If TypeCode is R
			- Read Length
			- Read Data(Length) as byte
	- Until MAX_CHILD_NODE_ITERATIONS or ChildNode.EndOffset == 0
		- Read ChildNode (goto 104)
	- If pos != EndOffset
		- Skip 9 bytes
*/