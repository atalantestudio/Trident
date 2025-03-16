#include "pch.hpp"

#include "Engine/Filesystem/HuffmanCodeDictionary.hpp"

namespace ProjectA {
	uint32 HuffmanCodeDictionary::reverseBits(uint32 value, uint8 count) {
		uint32 result = 0;

		for (uint8 bitIndex = 0; bitIndex <= (count / 2); bitIndex += 1) {
			const uint8 inverse = count - (bitIndex + 1);

			result |= ((value >> bitIndex) & 0x1) << inverse;
			result |= ((value >> inverse) & 0x1) << bitIndex;
		}

		return result;
	}

	HuffmanCodeDictionary::HuffmanCodeDictionary(uint8 maxCodeBitLength) :
		maxCodeBitLength(maxCodeBitLength)
	{
		ENGINE_ASSERT(maxCodeBitLength <= MAX_CODE_BIT_LENGTH, "The maximum code bit length is overflowing.");

		codeEntryCount = 1 << maxCodeBitLength;
		codeEntries = new HuffmanCodeEntry[codeEntryCount];
	}

	HuffmanCodeDictionary::~HuffmanCodeDictionary() {
		delete[] codeEntries;
	}

	void HuffmanCodeDictionary::compute(const uint16* codeLengths, uint32 symbolCount) {
		// 1. Count the number of codes for each code length.
		// Let codeBitLengthCodeCount[N] be the number of codes of length N, N >= 1.
		uint32 codeBitLengthCodeCount[MAX_CODE_BIT_LENGTH] = {};

		for (uint32 i = 0; i < symbolCount; i += 1) {
			const uint16 codeBitLength = codeLengths[i];

			ENGINE_ASSERT(codeBitLength < MAX_CODE_BIT_LENGTH, "The code bit length is overflowing.");

			codeBitLengthCodeCount[codeBitLength] += 1;
		}

		codeBitLengthCodeCount[0] = 0;

		// 2. Find the numerical value of the smallest code for each code length.
		uint32 codeBitLengthNextUnusedCodes[MAX_CODE_BIT_LENGTH] = {};

		for (uint8 i = 1; i < MAX_CODE_BIT_LENGTH; i += 1) {
			codeBitLengthNextUnusedCodes[i] = (codeBitLengthNextUnusedCodes[i - 1] + codeBitLengthCodeCount[i - 1]) << 1;
		}

		// 3. Assign numerical values to all codes, using consecutive values
		// for all codes of the same length with the base values determined at step 2.
		for (uint32 symbolIndex = 0; symbolIndex < symbolCount; symbolIndex += 1) {
			const uint8 codeBitLength = static_cast<uint8>(codeLengths[symbolIndex]);

			// Codes that are never used (which have a bit length of zero)
			// must not be assigned a value.
			if (codeBitLength == 0) {
				continue;
			}

			ENGINE_ASSERT(codeBitLength < MAX_CODE_BIT_LENGTH, "The code bit length is overflowing.");

			const uint32 nextCode = codeBitLengthNextUnusedCodes[codeBitLength];

			// Advance next unused code.
			codeBitLengthNextUnusedCodes[codeBitLength] += 1;

			const uint32 arbitraryBits = maxCodeBitLength - codeBitLength;
			const uint32 codeEntryCount = (1 << arbitraryBits);

			for (uint32 j = 0; j < codeEntryCount; j += 1) {
				const uint32 baseCodeEntryIndex = (nextCode << arbitraryBits) | j;
				const uint32 codeEntryIndex = reverseBits(baseCodeEntryIndex, maxCodeBitLength);

				HuffmanCodeEntry& codeEntry = codeEntries[codeEntryIndex];

				codeEntry.symbolIndex = static_cast<uint16>(symbolIndex);
				codeEntry.bitLength = codeBitLength;
			}
		}
	}

	uint16 HuffmanCodeDictionary::decode(BitStream& stream) const {
		// Read as many bits as possible to get a code entry index.
		const uint32 codeEntryIndex = stream.peek<uint32>(maxCodeBitLength);

		ENGINE_ASSERT(codeEntryIndex < codeEntryCount, "The code entry index is overflowing.");

		const HuffmanCodeEntry& codeEntry = codeEntries[codeEntryIndex];

		ENGINE_ASSERT(codeEntry.bitLength > 0, "Bit length for code entry is 0.");

		// TODO: Populate code entries.
		stream.jump(codeEntry.bitLength);

		return codeEntry.symbolIndex;
	}
}