#include "pch.hpp"

#include "Engine/Compression/HuffmanCodeDictionary.hpp"

namespace ProjectA {
	HuffmanCodeDictionary::HuffmanCodeDictionary(uint8 maxCodeLength) :
		maxCodeLength(maxCodeLength)
	{
		ENGINE_ASSERT(maxCodeLength < MAX_CODE_LENGTH, "The maximum code length is overflowing.");

		dictionary.resize(static_cast<std::size_t>(1) << maxCodeLength);
	}

	void HuffmanCodeDictionary::compute(const uint8* codeLengths, uint16 symbolCount) {
		// 1. Count the number of codes for each code length.
		// Let codeLengthCodeCount[N] be the number of codes of length N, N >= 1.
		uint16 codeLengthCodeCount[MAX_CODE_LENGTH] = {};

		for (uint16 symbolIndex = 0; symbolIndex < symbolCount; symbolIndex += 1) {
			const uint8 codeLength = codeLengths[symbolIndex];

			ENGINE_ASSERT(codeLength < MAX_CODE_LENGTH, "The code length is overflowing.");

			codeLengthCodeCount[codeLength] += 1;
		}

		codeLengthCodeCount[0] = 0;

		// 2. Find the numerical value of the smallest code for each code length.
		uint16 nextUnusedCode[MAX_CODE_LENGTH] = {};

		for (uint8 bitIndex = 1; bitIndex < MAX_CODE_LENGTH; bitIndex += 1) {
			nextUnusedCode[bitIndex] = (nextUnusedCode[bitIndex - 1] + codeLengthCodeCount[bitIndex - 1]) << 1;
		}

		// 3. Assign numerical values to all codes, using consecutive values
		// for all codes of the same length with the base values determined at step 2.
		for (uint16 symbolIndex = 0; symbolIndex < symbolCount; symbolIndex += 1) {
			const uint8 codeLength = codeLengths[symbolIndex];

			// Codes that are never used (which have a bit length of zero)
			// must not be assigned a value.
			if (codeLength == 0) {
				continue;
			}

			ENGINE_ASSERT(codeLength < MAX_CODE_LENGTH, "The code length is overflowing.");

			const uint16 code = nextUnusedCode[codeLength];

			// Advance next unused code.
			nextUnusedCode[codeLength] += 1;

			const uint32 arbitraryBits = maxCodeLength - codeLength;
			const uint32 entryCount = 1 << arbitraryBits;

			for (uint32 entryIndex = 0; entryIndex < entryCount; entryIndex += 1) {
				const uint32 baseIndex = (code << arbitraryBits) | entryIndex;
				const uint32 index = reverseBits(baseIndex, maxCodeLength);

				HuffmanCode& huffmanCode = dictionary[index];

				huffmanCode.code = symbolIndex;
				huffmanCode.bitLength = codeLength;
			}
		}
	}

	uint16 HuffmanCodeDictionary::decode(BitStream& stream) const {
		// Read as many bits as possible to get a code entry index.
		const uint16 codeIndex = stream.peek<uint16>(maxCodeLength);

		ENGINE_ASSERT(codeIndex < dictionary.size(), "The code index is overflowing.");

		const HuffmanCode& huffmanCode = dictionary[codeIndex];

		ENGINE_ASSERT(huffmanCode.bitLength > 0, "The bit length is 0.");

		stream.jump(huffmanCode.bitLength);

		return huffmanCode.code;
	}

	uint32 HuffmanCodeDictionary::reverseBits(uint32 value, uint8 bitCount) {
		uint32 result = 0;

		for (uint8 bitIndex = 0; bitIndex <= (bitCount / 2); bitIndex += 1) {
			const uint8 inverse = (bitCount - (bitIndex + 1));

			result |= ((value >> bitIndex) & 0x1) << inverse;
			result |= ((value >> inverse) & 0x1) << bitIndex;
		}

		return result;
	}
}