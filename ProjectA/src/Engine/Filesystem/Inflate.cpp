#include "pch.hpp"

#include "Engine/Filesystem/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/Inflate.hpp"

namespace ProjectA {
	chain<uint8> inflate(BitStream& stream, uint64 uncompressedSize) {
		/**
		 * CMF (Compression Method and flags)
		 * 
		 * This byte is divided into a 4-bit compression method and a 4-
		 * bit information field depending on the compression method.
		 * 
		 *    bits 0 to 3  CM     Compression method
		 *    bits 4 to 7  CINFO  Compression info
		 */
		const uint8 CMF = stream.read<uint8>(8);

		/**
		 * CM (Compression method)
		 * 
		 * This identifies the compression method used in the file. CM = 8
		 * denotes the "deflate" compression method with a window size up
		 * to 32K. This is the method used by gzip and PNG (see
		 * references [1] and [2] in Chapter 3, below, for the reference
		 * documents). CM = 15 is reserved. It might be used in a future
		 * version of this specification to indicate the presence of an
		 * extra field before the compressed data.
		 */
		const uint8 CM = CMF & 0b00001111;

		ENGINE_ASSERT(CM != 15, "CM = 15 is a reserved value. See https://datatracker.ietf.org/doc/html/rfc1950.");
		ENGINE_ASSERT(CM == 8, "CM = [] is an unhandled value.", static_cast<sint32>(CM));

		/**
		 * CINFO (Compression info)
		 * 
		 * For CM = 8, CINFO is the base-2 logarithm of the LZ77 window
		 * size, minus eight (CINFO=7 indicates a 32K window size). Values
		 * of CINFO above 7 are not allowed in this version of the
		 * specification. CINFO is not defined in this specification for
		 * CM not equal to 8.
		 */
		const uint8 CINFO = CMF >> 4;

		ENGINE_ASSERT(CINFO <= 7, "CINFO = [] is not allowed. See https://datatracker.ietf.org/doc/html/rfc1950.", static_cast<sint32>(CINFO));

		/**
		 * FLG (FLaGs)
		 * 
		 * This flag byte is divided as follows:
		 * 
		 *    bits 0 to 4  FCHECK  (check bits for CMF and FLG)
		 *    bit  5       FDICT   (preset dictionary)
		 *    bits 6 to 7  FLEVEL  (compression level)
		 */
		const uint8 FLG = stream.read<uint8>(8);

		/**
		 * FCHECK (Check bits for CMF and FLG)
		 * 
		 * The FCHECK value must be such that CMF and FLG, when viewed as
		 * a 16-bit unsigned integer stored in MSB order (CMF * 256 + FLG),
		 * is a multiple of 31.
		 */
		const uint8 FCHECK = FLG & 0b00011111;

		ENGINE_ASSERT((CMF * 256 + FLG) % 31 == 0, "FCHECK check failed. See https://datatracker.ietf.org/doc/html/rfc1950.");

		/**
		 * FDICT (Preset dictionary)
		 * 
		 * If FDICT is set, a DICT dictionary identifier is present
		 * immediately after the FLG byte. The dictionary is a sequence of
		 * bytes which are initially fed to the compressor without
		 * producing any compressed output. DICT is the Adler-32 checksum
		 * of this sequence of bytes (see the definition of ADLER32
		 * below). The decompressor can use this identifier to determine
		 * which dictionary has been used by the compressor.
		 */
		const uint8 FDICT = (FLG >> 5) & 0b00000001;

		ENGINE_ASSERT(FDICT == 0, "FDICT = [] is an unhandled value.", static_cast<sint32>(FDICT));

		/**
		 * FLEVEL (Compression level)
		 * 
		 * These flags are available for use by specific compression
		 * methods. The "deflate" method (CM = 8) sets these flags as
		 * follows:
		 * 
		 *    0 - compressor used fastest algorithm
		 *    1 - compressor used fast algorithm
		 *    2 - compressor used default algorithm
		 *    3 - compressor used maximum compression, slowest algorithm
		 * 
		 * The information in FLEVEL is not needed for decompression; it
		 * is there to indicate if recompression might be worthwhile.
		 */
		const uint8 FLEVEL = FLG >> 6;

		chain<uint8> data;

		data.reserve(uncompressedSize);

		// TODO: Compute theoretical max. number of blocks.
		constexpr uint32 MAX_BLOCK_COUNT = 1000;

		for (uint32 blockIndex = 0; blockIndex < MAX_BLOCK_COUNT; blockIndex += 1) {
			/**
			 * BFINAL is set if and only if this is the last block of the data
			 * set.
			 */
			const bool BFINAL = stream.read<uint8>(1);

			/**
			 * BTYPE specifies how the data are compressed, as follows:
			 * 
			 *	   00 - no compression
			 *	   01 - compressed with fixed Huffman codes
			 *	   10 - compressed with dynamic Huffman codes
			 *	   11 - reserved (error)
			 *
			 *	The only difference between the two compressed cases is how the
			 *	Huffman codes for the literal/length and distance alphabets are
			 *	defined.
			 */
			const uint8 BTYPE = stream.read<uint8>(2);

			ENGINE_ASSERT(BTYPE != 3, "BTYPE = 3 is a reserved value. See https://datatracker.ietf.org/doc/html/rfc1951.");

			if (BTYPE == 0) {
				parseUncompressedZlibBlock(stream, data);
			}
			else {
				parseCompressedZlibBlock(stream, BTYPE, data);
			}

			if (BFINAL) {
				return data;
			}
		}

		ENGINE_WARN("Reached last safe iteration.");

		return data;
	}

	void parseUncompressedZlibBlock(BitStream& stream, chain<uint8>& data) {
		ENGINE_DEBUG("Parsing uncompressed Zlib block.");

		stream.jump(5);

		/**
		 * LEN is the number of data bytes in the block.
		 */
		const uint16 LEN = stream.read<uint16>(16);

		/**
		 * NLEN is the one's complement of LEN.
		 */
		const uint16 NLEN = stream.read<uint16>(16);

		ENGINE_ASSERT(LEN == static_cast<uint16>(~NLEN), "NLEN check failed. See https://datatracker.ietf.org/doc/html/rfc1951.");

		const uint8* source = stream.getBytePosition();

		std::copy(source, source + LEN, std::back_inserter(data));
	}

	void parseCompressedZlibBlock(BitStream& stream, uint8 BTYPE, chain<uint8>& data) {
		static constexpr uint16 MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT = 320;

		uint8 literalLengthCodeLengths[MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT] = {};

		uint16 literalLengthCodeCount;
		uint8 distanceCodeCount;

		HuffmanCodeDictionary literalLengthDictionary(15);
		HuffmanCodeDictionary distanceDictionary(15);

		if (BTYPE == 1) {
			ENGINE_DEBUG("Parsing Zlib block compressed with fixed Huffman codes.");

			literalLengthCodeCount = 288;
			distanceCodeCount = 32;

			std::fill(&literalLengthCodeLengths[0], &literalLengthCodeLengths[288], 8);
			std::fill(&literalLengthCodeLengths[144], &literalLengthCodeLengths[256], 9);
			std::fill(&literalLengthCodeLengths[256], &literalLengthCodeLengths[280], 7);
			std::fill(&literalLengthCodeLengths[288], &literalLengthCodeLengths[320], 5);
		}
		else {
			ENGINE_DEBUG("Parsing Zlib block compressed with dynamic Huffman codes.");

			// # of Literal/Length codes - 257 (257 - 286)
			const uint8 HLIT = stream.read<uint8>(5);

			ENGINE_ASSERT(HLIT <= 29, "HLIT = [] is overflowing.", HLIT);

			// # of Distance codes - 1 (1 - 32)
			const uint8 HDIST = stream.read<uint8>(5);

			// # of Code Length codes - 4 (4 - 19)
			const uint8 HCLEN = stream.read<uint8>(4);

			literalLengthCodeCount = HLIT + 257;
			distanceCodeCount = HDIST + 1;

			const uint8 codeLengthCodeCount = HCLEN + 4;

			static constexpr uint8 MAX_CODE_LENGTH_CODE_COUNT = 19;
			static constexpr uint8 reorder[MAX_CODE_LENGTH_CODE_COUNT] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

			uint8 literalLengthCodeLengthCodeLengths[MAX_CODE_LENGTH_CODE_COUNT] = {};

			for (uint8 i = 0; i < codeLengthCodeCount; i += 1) {
				literalLengthCodeLengthCodeLengths[reorder[i]] = stream.read<uint8>(3);
			}

			HuffmanCodeDictionary literalLengthCodeLengthDictionary(7);

			literalLengthCodeLengthDictionary.compute(literalLengthCodeLengthCodeLengths, MAX_CODE_LENGTH_CODE_COUNT);

			const uint16 literalLengthCodeLengthCount = literalLengthCodeCount + distanceCodeCount;
			uint16 literalLengthCodeLengthIndex = 0;

			ENGINE_ASSERT(literalLengthCodeLengthCount <= MAX_LITERAL_LENGTH_CODE_LENGTH_COUNT, "The literal/length code length count is overflowing.");

			for (uint16 literalLengthCodeLengthIndex = 0; literalLengthCodeLengthIndex < literalLengthCodeLengthCount;) {
				const uint8 encodedCodeLength = static_cast<uint8>(literalLengthCodeLengthDictionary.decode(stream));

				ENGINE_ASSERT(encodedCodeLength <= 18, "Code length = [] is not allowed.", encodedCodeLength);

				uint8 codeLength = 0;
				uint8 repeatLength = 1;

				if (encodedCodeLength <= 15) {
					codeLength = encodedCodeLength;
				}
				else if (encodedCodeLength == 16) {
					ENGINE_ASSERT(literalLengthCodeLengthIndex > 0, "The literal/length code length index cannot be 0 if the encoded code length is 16.");

					codeLength = literalLengthCodeLengths[literalLengthCodeLengthIndex - 1];
					repeatLength = stream.read<uint8>(2) + 3;

					ENGINE_ASSERT(repeatLength <= 6, "Repeat length = [] is overflowing.", repeatLength);
				}
				else if (encodedCodeLength == 17) {
					repeatLength = stream.read<uint8>(3) + 3;

					ENGINE_ASSERT(repeatLength <= 10, "Repeat length = [] is overflowing.", repeatLength);
				}
				else if (encodedCodeLength == 18) {
					repeatLength = stream.read<uint8>(7) + 11;

					ENGINE_ASSERT(repeatLength <= 138, "Repeat length = [] is overflowing.", repeatLength);
				}

				std::fill(&literalLengthCodeLengths[literalLengthCodeLengthIndex], &literalLengthCodeLengths[literalLengthCodeLengthIndex + repeatLength], codeLength);

				literalLengthCodeLengthIndex += repeatLength;
			}
		}

		literalLengthDictionary.compute(literalLengthCodeLengths, literalLengthCodeCount);
		distanceDictionary.compute(literalLengthCodeLengths + literalLengthCodeCount, distanceCodeCount);

		// TODO: Compute theoretical max literal length count.
		constexpr uint32 MAX_LITERAL_LENGTH_COUNT = 100000;

		for (uint32 literalLengthIndex = 0; literalLengthIndex < MAX_LITERAL_LENGTH_COUNT; literalLengthIndex += 1) {
			const uint16 literalLength = literalLengthDictionary.decode(stream);

			ENGINE_ASSERT(literalLength <= 285, "Literal/length value should not exceed 285. See https://datatracker.ietf.org/doc/html/rfc1951.");

			if (literalLength <= 255) {
				data.emplace_back(static_cast<uint8>(literalLength));

				continue;
			}

			if (literalLength == 256) {
				return;
			}

			const uint16 lengthCodeIndex = literalLength - 257;

			ENGINE_ASSERT(lengthCodeIndex < 29, "The length code index is overflowing.");

			const LengthCode lengthCode = lengthCodes[lengthCodeIndex];
			const uint16 length = lengthCode.baseLength + stream.read<uint8>(lengthCode.extraBitCount);

			const uint16 distanceCodeIndex = distanceDictionary.decode(stream);

			ENGINE_ASSERT(distanceCodeIndex < 30, "The distance code index is overflowing.");

			const DistanceCode distanceCode = distanceCodes[distanceCodeIndex];
			const uint16 distance = distanceCode.baseDistance + stream.read<uint16>(distanceCode.extraBitCount);

			// TODO: Use iterator instead of raw pointer.
			//const chain<uint8>::const_iterator source = data.end() - distance;
			const uint8* source = data.data() + data.size() - distance;

			//ENGINE_ASSERT(source >= data.begin(), "The source iterator is underflowing.");
			ENGINE_ASSERT(source >= data.data(), "The source iterator is underflowing.");

			//std::copy(source, source + length, std::back_inserter(data));
			for (uint16 index = 0; index < length; index += 1) {
				data.emplace_back(*source);

				source += 1;
			}
		}

		ENGINE_WARN("Reached last safe iteration.");
	}
}