#include "pch.hpp"

#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Filesystem/PNGFileLoader.hpp"

namespace ProjectA {
	#define PNG_CHUNK_TYPE_STRING_TO_UINT32(type) ( \
		(static_cast<uint32>(type[0]) << 0) | \
		(static_cast<uint32>(type[1]) << 8) | \
		(static_cast<uint32>(type[2]) << 16) | \
		(static_cast<uint32>(type[3]) << 24) \
	)

	uint16 PNGFileLoader::reversedBits[1 << 13] = {};

	void PNGFileLoader::load(const std::string& path) {
		const chain<uint8> buffer = FileLoader::loadChain(path);
		ByteStream stream(buffer.data(), buffer.size());

		readSignature(stream);

		while (stream.getRemainingByteSize() != 0) {
			readChunk(stream);
		}

		ENGINE_TRACE("Done.");
	}

	void PNGFileLoader::readSignature(ByteStream& stream) {
		static constexpr uint8 SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

		for (uint8 i = 0; i < 8; i += 1) {
			ENGINE_ASSERT(stream.read<uint8>() == SIGNATURE[i], "PNGFileLoader: Failed to verify file signature.");
		}
	}

	void PNGFileLoader::readChunk(ByteStream& stream) {
		Chunk Chunk;
		Chunk.ByteLength = stream.read<uint32>(true);
		Chunk.Type = stream.read<uint32>();

		switch (Chunk.Type) {
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IHDR"):
				ENGINE_TRACE("Reading IHDR chunk.");

				readIHDR(stream);

				break;
			/*case PNG_CHUNK_TYPE_STRING_TO_UINT32("IDAT"):
				ENGINE_TRACE("Reading IDAT chunk.");

				readIDAT(stream);

				break;*/
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IEND"):
				ENGINE_TRACE("Reading IEND chunk.");

				readIEND(stream);

				break;
			default:
				char* TypeString = static_cast<char*>(static_cast<void*>(&Chunk.Type));

				ENGINE_WARN("Ignored unhandled chunk \"[][][][]\".", TypeString[0], TypeString[1], TypeString[2], TypeString[3]);

				stream.jump(Chunk.ByteLength);

				break;
		}

		Chunk.CRC = stream.read<uint32>(true);
	}

	void PNGFileLoader::readIHDR(ByteStream& stream) {
		const uint32 Width = stream.read<uint32>(true);

		if (Width == 0) {
			throw std::runtime_error("Width must be > 0.");
		}

		const uint32 Height = stream.read<uint32>(true);

		if (Height == 0) {
			throw std::runtime_error("Height must be > 0.");
		}

		const uint8 BitDepth = stream.read<uint8>();

		if (BitDepth != 1 && BitDepth != 2 && BitDepth != 4 && BitDepth != 8 && BitDepth != 16) {
			throw std::runtime_error("Invalid bit depth value.");
		}

		const uint8 ColorType = stream.read<uint8>();

		if (ColorType != 0 && ColorType != 2 && ColorType != 3 && ColorType != 4 && ColorType != 6) {
			throw std::runtime_error("Invalid color type value.");
		}

		const uint8 CompressionMethod = stream.read<uint8>();

		if (CompressionMethod != 0) {
			throw std::runtime_error("Invalid compression method value (must be 0).");
		}

		const uint8 FilterMethod = stream.read<uint8>();

		if (FilterMethod != 0) {
			throw std::runtime_error("Invalid filter method value (must be 0).");
		}

		const uint8 InterlaceMethod = stream.read<uint8>();

		if (InterlaceMethod != 0 && InterlaceMethod != 1) {
			throw std::runtime_error("Invalid interlace method value (must be 0 or 1).");
		}
	}

	/*void PNGFileLoader::readIDAT(ByteStream& stream) {
		const CMF_FLG cmfFlg = file.read<CMF_FLG>();

		if (cmfFlg.CM != 8) {
			throw std::runtime_error("Invalid CM (must be 8).");
		}

		if (cmfFlg.FDICT != 0) {
			throw std::runtime_error("Preset dictionaries are not supported.");
		}

		for (uint16 i = 0; i < (1 << 13); i++) {
			reversedBits[i] = static_cast<uint16>(reverseBits(i, 13));
		}

		uint32 BFINAL = 0;
		chain<uint8> output;

		while (BFINAL == 0) {
			BFINAL = file.readBits(1);

			const uint32 BTYPE = file.readBits(2);

			Huffman litLenHuffman = buildHuffman(15);
			Huffman distHuffman = buildHuffman(15);

			switch (BTYPE) {
				case 0b00: {
					// No compression.

					file.discardBits(5);

					// LEN is the number of data bytes in the block.
					uint16 LEN = file.read<uint16>();

					// NLEN is the one's complement of LEN.
					const uint16 NLEN = file.read<uint16>();

					ENGINE_ASSERT(LEN == ~NLEN, "LEN/~NLEN mismatch.");

					File::swapEndianness(&LEN);

					const chain<uint8> uncompressed = file.read<uint8>(LEN);

					for (const uint8 value : uncompressed) {
						output.emplace_back(value);
					}

					break;
				}
				case 0b01:
					ENGINE_DEBUG("todo: fixed hfc");

					break;
				case 0b10: {
					const uint32 HLIT = file.readBits(5) + 257;
					const uint32 HDIST = file.readBits(5) + 1;
					const uint32 HCLEN = file.readBits(4) + 4;

					ENGINE_ASSERT(HLIT <= 286, "Invalid HLIT value [].", HLIT);
					ENGINE_ASSERT(HDIST <= 32, "Invalid HDIST value [].", HDIST);
					ENGINE_ASSERT(HCLEN <= 19, "Invalid HCLEN value [].", HCLEN);

					static constexpr uint32 lut[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
					uint32 table[19] = {};

					for (uint32 i = 0; i < HCLEN; i++) {
						table[lut[i]] = file.readBits(3);
					}

					Huffman huffmanDict = buildHuffman(7);

					computeHuffman(huffmanDict, 19, table);

					uint32 LitLenDistTable[512] = {};
					uint32 LitLenCount = 0;
					uint32 LenCount = HLIT + HDIST;

					while (LitLenCount < LenCount) {
						uint32 EncodedLength = huffmanDecode(file, huffmanDict);
						uint32 RepeatLength = 1;
						uint32 CodeLength = 0;

						if (EncodedLength <= 15) {
							CodeLength = EncodedLength;
						}
						else if (EncodedLength == 16) {
							ENGINE_ASSERT(LitLenCount > 0, "LitLenCount is 0.");

							// Copy the previous code length 3 - 6 times.
							// The next 2 bits indicate repeat length.
							RepeatLength = file.readBits(2) + 3;
							CodeLength = LitLenDistTable[LitLenCount - 1];
						}
						else if (EncodedLength == 17) {
							// Repeat a code length of 0 for 3 - 10 times.
							// The next 3 bits indicate repeat length.
							RepeatLength = file.readBits(3) + 3;
						}
						else if (EncodedLength == 18) {
							// Repeat a code length of 0 for 11 - 138 times.
							// The next 7 bits indicate repeat length.
							RepeatLength = file.readBits(7) + 11;
						}
						else {
							throw std::runtime_error("Invalid EncodedLength value.");
						}

						while (RepeatLength--) {
							LitLenDistTable[LitLenCount] = CodeLength;
							LitLenCount++;
						}
					}

					ENGINE_ASSERT(LitLenCount == LenCount, "Invalid LitLenCount value [].", LitLenCount);

					computeHuffman(litLenHuffman, HLIT, LitLenDistTable);
					computeHuffman(distHuffman, HDIST, LitLenDistTable + HLIT);

					break;
				}
				case 0b11: {
					throw std::runtime_error("Reserved BTYPE value.");

					break;
				}
			}

			while (true) {
				const uint32 LitLen = huffmanDecode(file, litLenHuffman);

				if (LitLen <= 255) {
					// Output literal.
					output.emplace_back(static_cast<uint8>(LitLen & 0xFF));

					continue;
				}

				if (LitLen == 256) {
					break;
				}

				ENGINE_ASSERT(LitLen <= 285, "LitLen value is too large.");

				const uint32 LengthIndex = LitLen - 257;
				const HuffmanEntry LengthEntry = lengthExtra[LengthIndex];
				uint32 Length = LengthEntry.symbol;

				if (LengthEntry.usedBits > 0) {
					uint32 extraLengthBits = file.readBits(LengthEntry.usedBits);
					extraLengthBits = reverseBits(extraLengthBits, LengthEntry.usedBits);

					Length += extraLengthBits;
				}

				const uint32 DistanceIndex = huffmanDecode(file, distHuffman);
				const HuffmanEntry DistanceEntry = distanceExtra[DistanceIndex];
				uint32 Distance = DistanceEntry.symbol;

				if (DistanceEntry.usedBits > 0) {
					uint32 extraLengthBits = file.readBits(DistanceEntry.usedBits);
					extraLengthBits = reverseBits(extraLengthBits, DistanceEntry.usedBits);

					Distance += extraLengthBits;

					uint8* source = &output.back() - Distance;

					ENGINE_ASSERT(*source < output.size(), "Source pointer is overflowing.");

					while (Length--) {
						output.emplace_back(*source);

						source++;
					}
				}
			}
		}
	}*/

	/*Huffman PNGFileLoader::buildHuffman(uint32 maxCodeBitLength) {
		ENGINE_ASSERT(maxCodeBitLength > 0 && maxCodeBitLength <= 16, "maxCodeBitLength too small or too large.");

		Huffman huffman;
		huffman.maxCodeBitLength = maxCodeBitLength;
		huffman.entryCount = 1 << maxCodeBitLength;
		huffman.entries.resize(huffman.entryCount);

		return huffman;
	}*/

	/*void PNGFileLoader::computeHuffman(Huffman& huffman, uint32 symbolCount, uint32* codeLengths) {
		uint32 bl_count[16] = {};

		// 1.
		for (uint32 i = 0; i < symbolCount; i++) {
			const uint32 codeLength = codeLengths[i];

			ENGINE_ASSERT(codeLength < 16, "Overflowing code length.");

			bl_count[codeLength] += 1;
		}

		bl_count[0] = 0;

		// 2. Find the numerical value of the smallest code for each code length.
		uint32 next_code[16] = {0};

		for (uint32 bits = 1; bits < 16; bits++) {
			next_code[bits] = (next_code[bits - 1] + bl_count[bits - 1]) << 1;
		}

		// 3.
		for (uint32 i = 0; i < symbolCount; i++) {
			const uint32 codeBitLength = codeLengths[i];

			if (codeBitLength == 0) {
				continue;
			}

			ENGINE_ASSERT(codeBitLength < 16, "Overflowing code length.");

			const uint32 code = next_code[codeBitLength];

			next_code[codeBitLength] += 1;

			const uint32 arbitraryBits = huffman.maxCodeBitLength - codeBitLength;
			const uint32 entryCount = (1 << arbitraryBits);

			for (uint32 j = 0; j < entryCount; j++) {
				const uint32 baseIndex = (j << codeBitLength) | code;
				const uint32 index = reverseBits(baseIndex, huffman.maxCodeBitLength);

				huffman.entries[index].usedBits = codeBitLength;
				huffman.entries[index].symbol = i;
			}
		}
	}*/

	/*uint32 PNGFileLoader::huffmanDecode(File& file, const Huffman& huffman) {
		// Read as many bits as possible for a code.
		const uint32 Index = file.peekBits(huffman.maxCodeBitLength);

		ENGINE_ASSERT(Index < huffman.entries.size(), "The decoding index is overflowing.");

		const HuffmanEntry Entry = huffman.entries[Index];

		file.discardBits(Entry.usedBits);

		return Entry.symbol;
	}*/

	// TODO: Replace with intrinsic.
	/*uint32 PNGFileLoader::reverseBits(uint32 value, uint32 bitCount) {
		uint32 reversedValue = 0;

		for (uint32 bitIndex = 0; bitIndex < (bitCount * 0.5); bitIndex++) {
			const uint32 inverse = bitCount - (bitIndex + 1);

			reversedValue |= ((value >> bitIndex) & 0x1) << inverse;
			reversedValue |= ((value >> inverse) & 0x1) << bitIndex;
		}

		return reversedValue;
	}*/

	void PNGFileLoader::readIEND(ByteStream& stream) {
		ENGINE_WARN("TODO: Handle IEND chunk.");
	}
}