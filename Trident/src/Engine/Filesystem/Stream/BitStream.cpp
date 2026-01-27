#include "pch.hpp"

#include "Engine/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	struct LengthCode {
		uint16 baseLength;
		uint8 extraBitCount;
	};

	struct DistanceCode {
		uint16 baseDistance;
		uint8 extraBitCount;
	};

	static constexpr LengthCode lengthCodes[] = {
		{3, 0},
		{4, 0},
		{5, 0},
		{6, 0},
		{7, 0},
		{8, 0},
		{9, 0},
		{10, 0},
		{11, 1},
		{13, 1},
		{15, 1},
		{17, 1},
		{19, 2},
		{23, 2},
		{27, 2},
		{31, 2},
		{35, 3},
		{43, 3},
		{51, 3},
		{59, 3},
		{67, 4},
		{83, 4},
		{99, 4},
		{115, 4},
		{131, 5},
		{163, 5},
		{195, 5},
		{227, 5},
		{258, 0},
	};

	static constexpr DistanceCode distanceCodes[] = {
		{1, 0},
		{2, 0},
		{3, 0},
		{4, 0},
		{5, 1},
		{7, 1},
		{9, 2},
		{13, 2},
		{17, 3},
		{25, 3},
		{33, 4},
		{49, 4},
		{65, 5},
		{97, 5},
		{129, 6},
		{193, 6},
		{257, 7},
		{385, 7},
		{513, 8},
		{769, 8},
		{1025, 9},
		{1537, 9},
		{2049, 10},
		{3073, 10},
		{4097, 11},
		{6145, 11},
		{8193, 12},
		{12289, 12},
		{16385, 13},
		{24577, 13},
	};

	void BitStream<StreamOperation::READ>::parseUncompressedZlibBlock(uint8* data) {
		jump(5);

		// LEN is the number of data bytes in the block.
		const uint16 LEN = read<uint16>(16);

		// NLEN is the one's complement of LEN.
		const uint16 NLEN = read<uint16>(16);

		ENGINE_ASSERT(LEN == static_cast<uint16>(~NLEN), "NLEN check failed. See https://datatracker.ietf.org/doc/html/rfc1951.");

		std::copy_n(getBytePosition(), LEN, data);

		jump(LEN * 8);

		data += LEN;
	}

	void BitStream<StreamOperation::READ>::parseCompressedZlibBlock(uint8* data, uint64& dataOffset, uint8 BTYPE) {
		static constexpr uint16 MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT = 320;

		uint8 literalLengthCodeLengths[MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT] = {};

		uint16 literalLengthCodeCount;
		uint8 distanceCodeCount;

		HuffmanCodeDictionary literalLengthDictionary(15);
		HuffmanCodeDictionary distanceDictionary(15);

		if (BTYPE == 1) {
			literalLengthCodeCount = 288;
			distanceCodeCount = 32;

			std::fill(&literalLengthCodeLengths[0], &literalLengthCodeLengths[288], 8);
			std::fill(&literalLengthCodeLengths[144], &literalLengthCodeLengths[256], 9);
			std::fill(&literalLengthCodeLengths[256], &literalLengthCodeLengths[280], 7);
			std::fill(&literalLengthCodeLengths[288], &literalLengthCodeLengths[320], 5);
		}
		else {
			// # of Literal/Length codes - 257 (257 - 286)
			const uint8 HLIT = read<uint8>(5);

			ENGINE_ASSERT(HLIT <= 29, "HLIT = [] is overflowing.", HLIT);

			// # of Distance codes - 1 (1 - 32)
			const uint8 HDIST = read<uint8>(5);

			// # of Code Length codes - 4 (4 - 19)
			const uint8 HCLEN = read<uint8>(4);

			literalLengthCodeCount = HLIT + 257;
			distanceCodeCount = HDIST + 1;

			const uint8 codeLengthCodeCount = HCLEN + 4;

			static constexpr uint8 MAX_CODE_LENGTH_CODE_COUNT = 19;
			static constexpr uint8 reorder[MAX_CODE_LENGTH_CODE_COUNT] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

			uint8 literalLengthCodeLengthCodeLengths[MAX_CODE_LENGTH_CODE_COUNT] = {};

			for (uint8 i = 0; i < codeLengthCodeCount; i += 1) {
				literalLengthCodeLengthCodeLengths[reorder[i]] = read<uint8>(3);
			}

			HuffmanCodeDictionary literalLengthCodeLengthDictionary(7);

			literalLengthCodeLengthDictionary.compute(literalLengthCodeLengthCodeLengths, MAX_CODE_LENGTH_CODE_COUNT);

			const uint16 literalLengthCodeLengthCount = literalLengthCodeCount + distanceCodeCount;
			uint16 literalLengthCodeLengthIndex = 0;

			ENGINE_ASSERT(literalLengthCodeLengthCount <= MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT, "The literal/length code length count is overflowing.");

			for (uint16 literalLengthCodeLengthIndex = 0; literalLengthCodeLengthIndex < literalLengthCodeLengthCount;) {
				const uint8 encodedCodeLength = static_cast<uint8>(literalLengthCodeLengthDictionary.decode(*this));

				ENGINE_ASSERT(encodedCodeLength <= 18, "Code length = [] is not allowed.", encodedCodeLength);

				uint8 codeLength = 0;
				uint8 repeatLength = 1;

				if (encodedCodeLength <= 15) {
					codeLength = encodedCodeLength;
				}
				else if (encodedCodeLength == 16) {
					ENGINE_ASSERT(literalLengthCodeLengthIndex > 0, "The literal/length code length index cannot be 0 if the encoded code length is 16.");

					codeLength = literalLengthCodeLengths[literalLengthCodeLengthIndex - 1];
					repeatLength = read<uint8>(2) + 3;

					ENGINE_ASSERT(repeatLength <= 6, "Repeat length = [] is overflowing.", repeatLength);
				}
				else if (encodedCodeLength == 17) {
					repeatLength = read<uint8>(3) + 3;

					ENGINE_ASSERT(repeatLength <= 10, "Repeat length = [] is overflowing.", repeatLength);
				}
				else if (encodedCodeLength == 18) {
					repeatLength = read<uint8>(7) + 11;

					ENGINE_ASSERT(repeatLength <= 138, "Repeat length = [] is overflowing.", repeatLength);
				}

				std::fill(&literalLengthCodeLengths[literalLengthCodeLengthIndex], &literalLengthCodeLengths[literalLengthCodeLengthIndex + repeatLength], codeLength);

				literalLengthCodeLengthIndex += repeatLength;
			}
		}

		literalLengthDictionary.compute(literalLengthCodeLengths, literalLengthCodeCount);
		distanceDictionary.compute(literalLengthCodeLengths + literalLengthCodeCount, distanceCodeCount);

		// TODO: Compute theoretical max literal length count.
		static constexpr uint32 MAX_LITERAL_LENGTH_COUNT = 177237;

		for (uint32 literalLengthIndex = 0; literalLengthIndex < MAX_LITERAL_LENGTH_COUNT; literalLengthIndex += 1) {
			const uint16 literalLength = literalLengthDictionary.decode(*this);

			ENGINE_ASSERT(literalLength <= 285, "Literal/length value should not exceed 285. See https://datatracker.ietf.org/doc/html/rfc1951.");

			if (literalLength <= 255) {
				data[dataOffset] = static_cast<uint8>(literalLength);
				dataOffset += 1;

				continue;
			}

			if (literalLength == 256) {
				return;
			}

			const uint16 lengthCodeIndex = literalLength - 257;

			ENGINE_ASSERT(lengthCodeIndex < 29, "The length code index is overflowing.");

			const LengthCode lengthCode = lengthCodes[lengthCodeIndex];
			const uint16 length = lengthCode.baseLength + read<uint8>(lengthCode.extraBitCount);

			const uint16 distanceCodeIndex = distanceDictionary.decode(*this);

			ENGINE_ASSERT(distanceCodeIndex < 30, "The distance code index is overflowing.");

			const DistanceCode distanceCode = distanceCodes[distanceCodeIndex];
			const uint16 distance = distanceCode.baseDistance + read<uint16>(distanceCode.extraBitCount);

			ENGINE_ASSERT(distance <= dataOffset, "The source pointer is not within the bounds of the uncompressed data.");

			// TODO: std::copy doesn't produce correct results for Cameraman.png, Zelda.png and a part of Pepper.png.
			#if 0
				std::copy_n(&data[dataOffset - distance], length, &data[dataOffset]);

				dataOffset += length;
			#else
				for (uint16 index = 0; index < length; index += 1) {
					data[dataOffset] = data[dataOffset - distance];
					dataOffset += 1;
				}
			#endif
		}

		ENGINE_WARNING("BitStream::inflate: Reached last safe iteration.");
	}
}