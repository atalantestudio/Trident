#include "pch.hpp"

#include "Engine/Filesystem/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Filesystem/Inflate.hpp"
#include "Engine/Filesystem/PNG/FilterType.hpp"
#include "Engine/Filesystem/PNGFileLoader.hpp"

namespace ProjectA {
	#define PNG_CHUNK_TYPE_STRING_TO_UINT32(type) ( \
		(static_cast<uint32>(type[0]) << 0) | \
		(static_cast<uint32>(type[1]) << 8) | \
		(static_cast<uint32>(type[2]) << 16) | \
		(static_cast<uint32>(type[3]) << 24) \
	)

	PNGFile PNGFileLoader::load(const std::string& path) {
		const File file = FileLoader::loadFile(path);
		ByteStream stream(file.rawData.data(), file.size);

		readSignature(stream);

		PNGFile pngFile;
		pngFile.name = file.name;
		pngFile.absolutePath = file.absolutePath;
		pngFile.size = file.size;
		pngFile.rawData = file.rawData;

		while (stream.getRemainingByteSize() > 0) {
			readChunk(stream, pngFile);
		}

		BitStream bitStream(pngFile.encodedData.data(), pngFile.encodedData.size());

		const uint32 finalSize = pngFile.width * pngFile.height * 4 + (pngFile.height * 1);

		const chain<uint8> unfilteredData = inflate(bitStream, finalSize);

		pngFile.data.resize(pngFile.width * pngFile.height * 4);

		const uint8* source = &unfilteredData[0];
		uint8* destination = &pngFile.data[0];

		const uint32 rowLength = pngFile.width * 4 + 1;

		const uint8 zero = 0;
		uint8* previousRow = (uint8*) &zero;
		sint32 previousRowAdvance = 0;

		// TODO: Refactor with SIMD (vec4<sint32>).
		for (uint32 y = 0; y < pngFile.height; y += 1) {
			const FilterType filterType = static_cast<FilterType>(*source++);

			uint8* currentRow = destination;

			switch (filterType) {
				case FilterType::NONE: {
					//ENGINE_DEBUG("Reconstructing unfiltered scanline [].", y);

					// TODO: Use std::copy here.
					// std::copy(&output[y * (properties.Height + 1) + 1], &output[y * (properties.Height + 1) + properties.Width + 1], finalOutput[y * properties.Height]);
					for (uint32 x = 0; x < pngFile.width; x += 1) {
						destination[0] = source[0];
						destination[1] = source[1];
						destination[2] = source[2];
						destination[3] = source[3];

						destination += 4;
						source += 4;
					}

					break;
				}
				case FilterType::SUB: {
					//ENGINE_DEBUG("Reconstructing scanline [] with sub filter.", y);

					uint32 aPixel = 0;

					for (uint32 x = 0; x < pngFile.width; x += 1) {
						destination[0] = applyAddFilter(source, (uint8*) &aPixel, 0);
						destination[1] = applyAddFilter(source, (uint8*) &aPixel, 1);
						destination[2] = applyAddFilter(source, (uint8*) &aPixel, 2);
						destination[3] = applyAddFilter(source, (uint8*) &aPixel, 3);

						aPixel = *(uint32*) destination;
						destination += 4;
						source += 4;
					}

					break;
				}
				case FilterType::UP: {
					//ENGINE_DEBUG("Reconstructing scanline [] with up filter.", y);

					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < pngFile.width; x += 1) {
						destination[0] = applyAddFilter(source, bPixel, 0);
						destination[1] = applyAddFilter(source, bPixel, 1);
						destination[2] = applyAddFilter(source, bPixel, 2);
						destination[3] = applyAddFilter(source, bPixel, 3);

						bPixel += previousRowAdvance;
						destination += 4;
						source += 4;
					}

					break;
				}
				case FilterType::AVERAGE: {
					//ENGINE_DEBUG("Reconstructing scanline [] with average filter.", y);

					uint32 aPixel = 0;
					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < pngFile.width; x += 1) {
						destination[0] = applyAverageFilter(source, (uint8*) &aPixel, bPixel, 0);
						destination[1] = applyAverageFilter(source, (uint8*) &aPixel, bPixel, 1);
						destination[2] = applyAverageFilter(source, (uint8*) &aPixel, bPixel, 2);
						destination[3] = applyAverageFilter(source, (uint8*) &aPixel, bPixel, 3);

						aPixel = *(uint32*) destination;
						bPixel += previousRowAdvance;
						destination += 4;
						source += 4;
					}

					break;
				}
				case FilterType::PAETH: {
					//ENGINE_DEBUG("Reconstructing scanline [] with Paeth filter.", y);

					uint32 aPixel = 0;
					uint8* bPixel = previousRow;
					uint32 cPixel = 0;

					for (uint32 x = 0; x < pngFile.width; x += 1) {
						destination[0] = applyPaethFilter(source, (uint8*) &aPixel, bPixel, (uint8*) &cPixel, 0);
						destination[1] = applyPaethFilter(source, (uint8*) &aPixel, bPixel, (uint8*) &cPixel, 1);
						destination[2] = applyPaethFilter(source, (uint8*) &aPixel, bPixel, (uint8*) &cPixel, 2);
						destination[3] = applyPaethFilter(source, (uint8*) &aPixel, bPixel, (uint8*) &cPixel, 3);

						aPixel = *(uint32*) destination;
						cPixel = *(uint32*) bPixel;
						bPixel += previousRowAdvance;
						destination += 4;
						source += 4;
					}

					break;
				}
				default:
					ENGINE_ASSERT(false, "Unhandled filter type [].", static_cast<uint16>(filterType));

					break;
			}

			previousRowAdvance = 4;
			previousRow = currentRow;
		}

		return pngFile;
	}

	void PNGFileLoader::readSignature(ByteStream& stream) {
		ENGINE_ASSERT(stream.read<uint8>() == 0x89, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x50, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x4E, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x47, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x0D, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x0A, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x1A, "Failed to verify signature.");
		ENGINE_ASSERT(stream.read<uint8>() == 0x0A, "Failed to verify signature.");
	}

	void PNGFileLoader::readChunk(ByteStream& stream, PNGFile& file) {
		Chunk Chunk;
		Chunk.ByteLength = stream.read<uint32>(true);
		Chunk.Type = stream.read<uint32>();

		switch (Chunk.Type) {
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IHDR"):
				ENGINE_TRACE("Reading IHDR chunk.");

				readIHDRChunk(stream, file);

				break;
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IDAT"):
				ENGINE_TRACE("Reading IDAT chunk.");

				readIDATChunk(stream, Chunk.ByteLength, file);

				break;
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IEND"):
				ENGINE_TRACE("Reading IEND chunk.");

				break;
			default:
				const char* TypeString = static_cast<char*>(static_cast<void*>(&Chunk.Type));

				ENGINE_WARN("Unhandled chunk \"[][][][]\".", TypeString[0], TypeString[1], TypeString[2], TypeString[3]);

				stream.jump(Chunk.ByteLength);

				break;
		}

		// TODO: Verify CRC. See https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix.
		Chunk.CRC = stream.read<uint32>(true);
	}

	void PNGFileLoader::readIHDRChunk(ByteStream& stream, PNGFile& file) {
		file.width = stream.read<uint32>(true);

		if (file.width == 0) {
			throw std::runtime_error("Width must be > 0.");
		}

		file.height = stream.read<uint32>(true);

		if (file.height == 0) {
			throw std::runtime_error("Height must be > 0.");
		}

		file.bitDepth = stream.read<uint8>();

		if (file.bitDepth != 1 && file.bitDepth != 2 && file.bitDepth != 4 && file.bitDepth != 8 && file.bitDepth != 16) {
			throw std::runtime_error("Invalid bit depth value.");
		}

		file.colorType = stream.read<uint8>();

		if (file.colorType != 0 && file.colorType != 2 && file.colorType != 3 && file.colorType != 4 && file.colorType != 6) {
			throw std::runtime_error("Invalid color type value.");
		}

		file.compressionMethod = stream.read<uint8>();

		if (file.compressionMethod != 0) {
			throw std::runtime_error("Invalid compression method value (must be 0).");
		}

		file.filterMethod = stream.read<uint8>();

		if (file.filterMethod != 0) {
			throw std::runtime_error("Invalid filter method value (must be 0).");
		}

		file.interlaceMethod = stream.read<uint8>();

		if (file.interlaceMethod != 0 && file.interlaceMethod != 1) {
			throw std::runtime_error("Invalid interlace method value (must be 0 or 1).");
		}

		// Current feature support.
		ENGINE_ASSERT(file.bitDepth == 8, "Only a bit depth of 8 is supported.");
		ENGINE_ASSERT(file.colorType == 6, "Only a color type of 6 (truecolor with alpha) is supported.");
	}

	void PNGFileLoader::readIDATChunk(ByteStream& stream, uint32 chunkByteLength, PNGFile& file) {
		file.encodedData.resize(file.encodedData.size() + chunkByteLength);

		std::copy(
			stream.getStream() + stream.getByteOffset(),
			stream.getStream() + stream.getByteOffset() + chunkByteLength,
			file.encodedData.end() - chunkByteLength
		);

		// Skip chunk data for the moment.
		stream.jump(chunkByteLength);
	}

	void PNGFileLoader::readZlibStream(BitStream& stream, uint8* destination, const uint8* firstDestination, const uint8* lastDestination) {
		// Compression method.
		const uint8 CM = stream.read<uint8>(4);

		ENGINE_ASSERT(CM == 8, "Unhandled CM value.");

		// Compression info.
		const uint8 CINFO = stream.read<uint8>(4);

		ENGINE_ASSERT(CINFO <= 7, "CINFO > 7 is not allowed.");

		// Check bits for CMF and FLG.
		const uint8 FCHECK = stream.read<uint8>(5);

		// Preset dictionary.
		const uint8 FDICT = stream.read<uint8>(1);

		ENGINE_ASSERT(FDICT == 0, "Preset dictionaries are not allowed by the PNG standard.");

		// Compression level.
		const uint8 FLEVEL = stream.read<uint8>(2);

		uint8 BFINAL = 0;

		while (BFINAL == 0) {
			readZlibBlock(stream, BFINAL, destination, firstDestination, lastDestination);
		}

		stream.close();
	}

	void PNGFileLoader::readZlibBlock(BitStream& stream, uint8& BFINAL, uint8* destination, const uint8* firstDestination, const uint8* lastDestination) {
		BFINAL = stream.read<uint8>(1);

		const uint8 BTYPE = stream.read<uint8>(2);

		ENGINE_DEBUG("Reading zlib block (BFINAL = [], BTYPE = []).", static_cast<sint32>(BFINAL), static_cast<sint32>(BTYPE));

		ENGINE_ASSERT(BTYPE < 3, "Reserved BTYPE value.");

		if (BTYPE == 0) {
			// No compression.

			stream.jump(5);

			// LEN is the number of data bytes in the block.
			uint16 LEN = stream.read<uint16>(16);

			// NLEN is the one's complement of LEN.
			const uint16 NLEN = stream.read<uint16>(16);

			ENGINE_ASSERT(LEN == static_cast<uint16>(~NLEN), "Failed to verify NLEN ([]) as the one's complement of LEN ([]).", NLEN, LEN);

			for (uint16 i = 0; i < LEN; i += 1) {
				*destination++ = stream.read<uint8>(8);
			}

			return;
		}

		// Fixed or dynamic Huffman codes.

		constexpr uint16 literalLengthDistanceTableCount = 512;

		uint8 literalLengthDistanceTable[literalLengthDistanceTableCount] = {};

		HuffmanCodeDictionary literalLengthDictionary(15);
		HuffmanCodeDictionary distanceDictionary(15);

		uint16 literalLengthCount;
		uint8 distanceCount;

		if (BTYPE == 1) {
			// Fixed Huffman codes.

			literalLengthCount = 288;
			distanceCount = 32;

			uint16 literalLengthIndex = 0;

			for (uint16 bitCountIndex = 0; bitCountIndex < 5; bitCountIndex += 1) {
				const BitCount bitCount = bitCounts[bitCountIndex];

				while (literalLengthIndex <= bitCount.maxLiteralLengthIndex) {
					literalLengthDistanceTable[literalLengthIndex] = bitCount.bitCount;
					literalLengthIndex += 1;
				}
			}
		}
		else {
			// Dynamic Huffman codes.

			// Number of Literal/Length codes - 257. (257 - 286)
			const uint16 HLIT = stream.read<uint16>(5);

			ENGINE_ASSERT(HLIT <= 29, "[] is an invalid HLIT value.", HLIT);

			// Number of Distance codes - 1. (1 - 32)
			const uint8 HDIST = stream.read<uint8>(5);

			// Number of Code Length codes - 4. (4 - 19)
			const uint8 HCLEN = stream.read<uint8>(4);

			literalLengthCount = HLIT + 257;
			distanceCount = HDIST + 1;

			static constexpr uint8 codeLengthIndexLookupTable[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

			// Code lengths for the code length alphabet,
			// in the order given by codeLengthIndexLookupTable.
			uint8 codeLengthCodeLengths[19] = {};

			for (uint8 i = 0; i < HCLEN + 4; i += 1) {
				codeLengthCodeLengths[codeLengthIndexLookupTable[i]] = stream.read<uint8>(3);
			}

			HuffmanCodeDictionary literalLengthCodeDictionary(7);

			literalLengthCodeDictionary.compute(codeLengthCodeLengths, 19);

			const uint16 lengthCount = literalLengthCount + distanceCount;
			uint16 literalLengthIndex = 0;

			ENGINE_ASSERT(lengthCount < literalLengthDistanceTableCount, "The length count is overflowing [] ([]).", lengthCount, literalLengthDistanceTableCount);

			while (literalLengthIndex < lengthCount) {
				const uint8 encodedCodeLength = static_cast<uint8>(literalLengthCodeDictionary.decode(stream));

				ENGINE_ASSERT(encodedCodeLength <= 18, "Unhandled encoded code length ([]).", encodedCodeLength);

				uint8 codeLength = 0;
				uint8 repeatLength = 1;

				if (encodedCodeLength <= 15) {
					// Represent code lengths of 0 - 15.
					codeLength = encodedCodeLength;
				}
				else if (encodedCodeLength == 16) {
					// Copy the previous code length 3 - 6 times.
					// The next 2 bits indicate repeat length.
					ENGINE_ASSERT(literalLengthIndex > 0, "Literal/length count is 0.");

					codeLength = literalLengthDistanceTable[literalLengthIndex - 1];
					repeatLength = stream.read<uint8>(2) + 3;

					ENGINE_ASSERT(repeatLength <= 6, "The repeat length is overflowing ([]).", repeatLength);
				}
				else if (encodedCodeLength == 17) {
					// Repeat a code length of 0 for 3 - 10 times.
					// The next 3 bits indicate repeat length.
					repeatLength = stream.read<uint8>(3) + 3;

					ENGINE_ASSERT(repeatLength <= 10, "The repeat length is overflowing ([]).", repeatLength);
				}
				else if (encodedCodeLength == 18) {
					// Repeat a code length of 0 for 11 - 138 times.
					// The next 7 bits indicate repeat length.
					repeatLength = stream.read<uint8>(7) + 11;

					ENGINE_ASSERT(repeatLength <= 138, "The repeat length is overflowing ([]).", repeatLength);
				}

				// TODO: Replace with memset.
				while (repeatLength--) {
					literalLengthDistanceTable[literalLengthIndex++] = codeLength;
				}
			}

			ENGINE_ASSERT(literalLengthIndex == lengthCount, "Literal/length count ([]) should equal the length count ([]).", literalLengthIndex, lengthCount);
		}

		literalLengthDictionary.compute(&literalLengthDistanceTable[0], literalLengthCount);
		distanceDictionary.compute(&literalLengthDistanceTable[literalLengthCount], distanceCount);

		// TODO: Set max iteration count.
		while (true) {
			const uint16 literalLength = literalLengthDictionary.decode(stream);

			if (literalLength <= 255) {
				// Output literal.
				*destination++ = static_cast<uint8>(literalLength);

				continue;
			}

			if (literalLength == 256) {
				// End of block.
				break;
			}

			ENGINE_ASSERT(literalLength <= 285, "Literal/length should not exceed 285.");

			const uint16 lengthExtraEntryIndex = literalLength - 257;
			const HuffmanCode& lengthExtraEntry = lengthExtra[lengthExtraEntryIndex];
			uint16 length = lengthExtraEntry.symbolIndex;

			if (lengthExtraEntry.bitLength > 0) {
				length += stream.read<uint16>(lengthExtraEntry.bitLength);
			}

			const uint16 distanceExtraEntryIndex = distanceDictionary.decode(stream);
			const HuffmanCode& distanceExtraEntry = distanceExtra[distanceExtraEntryIndex];
			uint16 distance = distanceExtraEntry.symbolIndex;

			if (distanceExtraEntry.bitLength > 0) {
				distance += stream.read<uint16>(distanceExtraEntry.bitLength);
			}

			const uint8* source = destination - distance;

			ENGINE_ASSERT(source + length <= lastDestination, "");
			ENGINE_ASSERT(destination + length <= lastDestination, "");
			ENGINE_ASSERT(source >= firstDestination, "The source index is underflowing.");

			while (length--) {
				*destination++ = *source++;
			}
		}
	}

	uint8 PNGFileLoader::applyAddFilter(const uint8* x, uint8* aOrB, uint8 channelIndex) {
		return ((uint8)x[channelIndex]) + ((uint8)aOrB[channelIndex]);
	}

	uint8 PNGFileLoader::applyAverageFilter(const uint8* x, uint8* a, uint8* b, uint8 channelIndex) {
		const uint32 average = ((uint32)a[channelIndex] + (uint32)b[channelIndex]) / 2;

		return (uint8) x[channelIndex] + (uint8)average;
	}

	// TODO: Use vec3<sint32>.
	uint8 PNGFileLoader::applyPaethFilter(const uint8* x, uint8* aPixel, uint8* bPixel, uint8* cPixel, uint8 channelIndex) {
		sint32 a = (sint32)aPixel[channelIndex];
		sint32 b = (sint32)bPixel[channelIndex];
		sint32 c = (sint32)cPixel[channelIndex];

		const sint32 p = a + b - c;

		sint32 pa = p - a;
		sint32 pb = p - b;
		sint32 pc = p - c;

		if (pa < 0) pa = -pa;
		if (pb < 0) pb = -pb;
		if (pc < 0) pc = -pc;

		sint32 paeth = c;

		if (pa <= pb && pa <= pc) {
			paeth = a;
		}
		else if (pb <= pc) {
			paeth = b;
		}

		return (uint8)x[channelIndex] + (uint8)paeth;
	}
}