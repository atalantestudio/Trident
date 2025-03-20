#include "pch.hpp"

#include "Engine/Filesystem/Compression/HuffmanCodeDictionary.hpp"

namespace ProjectA {
	uint32 HuffmanCodeDictionary::reverseBits(uint32 value, uint32 bitCount) {
		uint32 result = 0;

		for (uint32 bitIndex = 0; bitIndex <= (bitCount / 2); bitIndex += 1) {
			const uint32 inverse = bitCount - (bitIndex + 1);

			result |= ((value >> bitIndex) & 0x1) << inverse;
			result |= ((value >> inverse) & 0x1) << bitIndex;
		}

		return result;
	}

	HuffmanCodeDictionary::HuffmanCodeDictionary(uint8 maxCodeBitLength) :
		maxCodeBitLength(maxCodeBitLength)
	{
		ENGINE_ASSERT(maxCodeBitLength < MAX_CODE_BIT_LENGTH, "The maximum code bit length is overflowing.");

		const uint32 codeEntryCount = 1 << maxCodeBitLength;

		codeEntries.resize(codeEntryCount);
	}

	void HuffmanCodeDictionary::compute(const uint16* codeLengths, uint16 symbolCount) {
		// 1. Count the number of codes for each code length.
		// Let codeBitLengthCodeCount[N] be the number of codes of length N, N >= 1.
		uint16 codeBitLengthCodeCount[MAX_CODE_BIT_LENGTH] = {};

		for (uint16 symbolIndex = 0; symbolIndex < symbolCount; symbolIndex += 1) {
			const uint16 codeBitLength = codeLengths[symbolIndex];

			ENGINE_ASSERT(codeBitLength < MAX_CODE_BIT_LENGTH, "The code bit length is overflowing.");

			codeBitLengthCodeCount[codeBitLength] += 1;
		}

		codeBitLengthCodeCount[0] = 0;

		// 2. Find the numerical value of the smallest code for each code length.
		uint32 codeBitLengthNextUnusedCodes[MAX_CODE_BIT_LENGTH] = {};

		codeBitLengthNextUnusedCodes[0] = 0;

		for (uint32 bitIndex = 1; bitIndex < MAX_CODE_BIT_LENGTH; bitIndex += 1) {
			codeBitLengthNextUnusedCodes[bitIndex] = (codeBitLengthNextUnusedCodes[bitIndex - 1] + codeBitLengthCodeCount[bitIndex - 1]) << 1;
		}

		// 3. Assign numerical values to all codes, using consecutive values
		// for all codes of the same length with the base values determined at step 2.
		for (uint16 symbolIndex = 0; symbolIndex < symbolCount; symbolIndex += 1) {
			const uint16 codeBitLength = codeLengths[symbolIndex];

			// Codes that are never used (which have a bit length of zero)
			// must not be assigned a value.
			if (codeBitLength == 0) {
				continue;
			}

			ENGINE_ASSERT(codeBitLength < MAX_CODE_BIT_LENGTH, "The code bit length is overflowing.");

			const uint32 code = codeBitLengthNextUnusedCodes[codeBitLength];

			// Advance next unused code.
			codeBitLengthNextUnusedCodes[codeBitLength] += 1;

			const uint32 arbitraryBits = maxCodeBitLength - codeBitLength;
			const uint32 entryCount = 1 << arbitraryBits;

			for (uint32 entryIndex = 0; entryIndex < entryCount; entryIndex += 1) {
				const uint32 baseIndex = (code << arbitraryBits) | entryIndex;
				const uint32 index = reverseBits(baseIndex, maxCodeBitLength);

				HuffmanCodeEntry& entry = codeEntries[index];

				entry.symbolIndex = symbolIndex;
				entry.bitLength = codeBitLength;
			}
		}
	}

	uint16 HuffmanCodeDictionary::decode(BitStream& stream) const {
		// Read as many bits as possible to get a code entry index.
		const uint16 codeEntryIndex = stream.peek<uint16>(maxCodeBitLength);

		ENGINE_ASSERT(codeEntryIndex < codeEntries.size(), "The code entry index is overflowing.");

		const HuffmanCodeEntry& codeEntry = codeEntries[codeEntryIndex];

		ENGINE_ASSERT(codeEntry.bitLength > 0, "Bit length for code entry is 0.");

		stream.jump(codeEntry.bitLength);

		return codeEntry.symbolIndex;
	}
}