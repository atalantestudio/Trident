#include "pch.hpp"

#include "Engine/Filesystem/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/FileLoader.hpp"
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

		chain<uint8> finalOutput;
		PNGFile pngFile;
		pngFile.name = file.name;
		pngFile.absolutePath = file.absolutePath;
		pngFile.size = file.size;
		pngFile.rawData = file.rawData;

		while (stream.getRemainingByteSize() > 0) {
			readChunk(stream, pngFile);
		}

		return pngFile;
	}

	void PNGFileLoader::readSignature(ByteStream& stream) {
		static constexpr uint8 SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

		for (uint8 i = 0; i < 8; i += 1) {
			ENGINE_ASSERT(stream.read<uint8>() == SIGNATURE[i], "PNGFileLoader: Failed to verify file signature.");
		}
	}

	void PNGFileLoader::readChunk(ByteStream& stream, PNGFile& file) {
		Chunk Chunk;
		Chunk.ByteLength = stream.read<uint32>(true);
		Chunk.Type = stream.read<uint32>();

		switch (Chunk.Type) {
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IHDR"):
				ENGINE_TRACE("Reading IHDR chunk.");

				readIHDR(stream, file);

				break;
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IDAT"):
				ENGINE_TRACE("Reading IDAT chunk.");

				readIDAT(stream, Chunk.ByteLength, file);

				break;
			case PNG_CHUNK_TYPE_STRING_TO_UINT32("IEND"):
				ENGINE_TRACE("Reading IEND chunk.");

				readIEND(stream);

				break;
			default:
				char* TypeString = static_cast<char*>(static_cast<void*>(&Chunk.Type));

				ENGINE_WARN("Unhandled chunk \"[][][][]\".", TypeString[0], TypeString[1], TypeString[2], TypeString[3]);

				stream.jump(Chunk.ByteLength);

				break;
		}

		// TODO: Verify CRC.
		Chunk.CRC = stream.read<uint32>(true);
	}

	void PNGFileLoader::readIHDR(ByteStream& stream, PNGFile& file) {
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
	}

	void PNGFileLoader::readIDAT(ByteStream& byteStream, uint32 chunkByteLength, PNGFile& file) {
		BitStream bitStream(byteStream.getStream() + byteStream.getByteOffset(), chunkByteLength);

		chain<uint8> output = readZlibStream(bitStream);

		byteStream.jump(chunkByteLength);

		ENGINE_ASSERT(file.bitDepth == 8, "Only a bit depth of 8 is supported.");

		file.data.resize(static_cast<std::size_t>(file.width * file.height * 4));

		uint8* source = &output[0];
		uint8* destination = &file.data[0];

		// TODO: Refactor with SIMD (vec4<sint32>).
		for (uint32 y = 0; y < file.height; y += 1) {
			const FilterType filterType = static_cast<FilterType>(*source);

			source += 1;

			uint32 a = 0;
			uint32 b = 0;
			uint32 c = 0;

			switch (filterType) {
				case FilterType::NONE:
					ENGINE_DEBUG("FilterType::NONE");

					for (uint32 x = 0; x < file.width; x += 1) {
						*(uint32*) destination = *(uint32*) source;

						destination += 4;
						source += 4;
					}

					// std::copy(&output[y * (properties.Height + 1) + 1], &output[y * (properties.Height + 1) + properties.Width + 1], finalOutput[y * properties.Height]);

					break;
				case FilterType::SUB:
					ENGINE_DEBUG("TODO: FilterType::SUB");

					break;
				case FilterType::UP:
					ENGINE_DEBUG("TODO: FilterType::UP");

					break;
				case FilterType::AVERAGE:
					ENGINE_DEBUG("TODO: FilterType::AVERAGE");

					break;
				case FilterType::PAETH:
					ENGINE_DEBUG("TODO: FilterType::PAETH");

					break;
				default:
					ENGINE_ASSERT(false, "Unhandled filter type [].", static_cast<uint16>(filterType));

					break;
			}
		}
	}

	chain<uint8> PNGFileLoader::readZlibStream(BitStream& stream) {
		// Compression method.
		const uint8 CM = stream.read<uint8>(4);

		if (CM != 8) {
			throw std::runtime_error("Invalid CM (must be 8).");
		}

		// Compression info.
		const uint8 CINFO = stream.read<uint8>(4);

		// Check bits for CMF and FLG.
		const uint8 FCHECK = stream.read<uint8>(5);

		// Preset dictionary.
		const uint8 FDICT = stream.read<uint8>(1);

		if (FDICT != 0) {
			throw std::runtime_error("Preset dictionaries are not supported.");
		}

		// Compression level.
		const uint8 FLEVEL = stream.read<uint8>(2);

		chain<uint8> output;
		uint8 BFINAL = 0;

		while (BFINAL == 0) {
			readZlibBlock(stream, BFINAL, output);
		}

		stream.close();

		return output;
	}

	void PNGFileLoader::readZlibBlock(BitStream& stream, uint8& BFINAL, chain<uint8>& output) {
		BFINAL = stream.read<uint8>(1);

		ENGINE_DEBUG("BFINAL = []", static_cast<sint32>(BFINAL));

		const uint8 BTYPE = stream.read<uint8>(2);

		ENGINE_ASSERT(BTYPE != 0b11, "0b11 is a reserved BTYPE value.");

		HuffmanCodeDictionary literalLengthDictionary(15);
		HuffmanCodeDictionary distanceDictionary(15);

		switch (BTYPE) {
			case 0b00: {
				// No compression.
				ENGINE_DEBUG("No compression");

				stream.jump(5);

				// LEN is the number of data bytes in the block.
				uint16 LEN = stream.read<uint16>(16);

				// NLEN is the one's complement of LEN.
				const uint16 NLEN = stream.read<uint16>(16);

				ENGINE_ASSERT(LEN == static_cast<uint16>(~NLEN), "Failed to verify NLEN ([]) as the one's complement of LEN ([]).", NLEN, LEN);

				const uint8* source = stream.getBytePosition();

				const std::size_t previousSize = output.size();

				output.resize(previousSize + LEN);

				std::copy(source, source + LEN, &output[previousSize]);

				break;
			}
			case 0b01: {
				// Compressed with fixed Huffman codes.
				ENGINE_DEBUG("Fixed Huffman codes");

				// TODO
				ENGINE_ASSERT(false, "TODO");

				break;
			}
			case 0b10: {
				// Compressed with dynamic Huffman codes.
				ENGINE_DEBUG("Dynamic Huffman codes");

				static constexpr uint8 codeLengthIndexLookupTable[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

				// Number of Literal/Length codes - 257. (257 - 286)
				const uint16 HLIT = stream.read<uint8>(5) + 257;

				ENGINE_ASSERT(HLIT <= 286, "[] is an invalid HLIT value.", HLIT)

				// Number of Distance codes - 1. (1 - 32)
				const uint8 HDIST = stream.read<uint8>(5) + 1;

				ENGINE_ASSERT(HDIST <= 32, "[] is an invalid HDIST value.", HDIST);

				// Number of Code Length codes - 4. (4 - 19)
				const uint8 HCLEN = stream.read<uint8>(4) + 4;

				ENGINE_ASSERT(HCLEN <= 19, "[] is an invalid HCLEN value.", HCLEN);

				// Code lengths for the code length alphabet,
				// in the order given by codeLengthIndexLookupTable.
				uint16 codeLengthAlphabetCodeLengths[19] = {};

				for (uint8 i = 0; i < HCLEN; i += 1) {
					codeLengthAlphabetCodeLengths[codeLengthIndexLookupTable[i]] = stream.read<uint8>(3);
				}

				HuffmanCodeDictionary literalLengthCodeDictionary(7);

				literalLengthCodeDictionary.compute(codeLengthAlphabetCodeLengths, 19);

				const uint16 lengthCount = HLIT + HDIST;
				uint16 literalLengthCount = 0;
				uint16 literalLengthDistanceTable[512] = {};

				while (literalLengthCount < lengthCount) {
					const uint16 encodedCodeLength = literalLengthCodeDictionary.decode(stream);

					ENGINE_ASSERT(encodedCodeLength <= 18, "Unhandled encoded code length ([]).", encodedCodeLength);

					uint16 codeLength = 0;
					uint16 repeatLength = 1;

					if (encodedCodeLength <= 15) {
						// Represent code lengths of 0 - 15.
						codeLength = encodedCodeLength;
					}
					else if (encodedCodeLength == 16) {
						// Copy the previous code length 3 - 6 times.
						// The next 2 bits indicate repeat length.
						ENGINE_ASSERT(literalLengthCount > 0, "Literal/length count is 0.");

						codeLength = literalLengthDistanceTable[literalLengthCount - 1];
						repeatLength = stream.read<uint8>(2) + 3;
					}
					else if (encodedCodeLength == 17) {
						// Repeat a code length of 0 for 3 - 10 times.
						// The next 3 bits indicate repeat length.
						repeatLength = stream.read<uint8>(3) + 3;
					}
					else if (encodedCodeLength == 18) {
						// Repeat a code length of 0 for 11 - 138 times.
						// The next 7 bits indicate repeat length.
						repeatLength = stream.read<uint8>(7) + 11;
					}

					while (repeatLength > 0) {
						literalLengthDistanceTable[literalLengthCount] = codeLength;
						literalLengthCount += 1;
						repeatLength -= 1;
					}
				}

				ENGINE_ASSERT(literalLengthCount == lengthCount, "Literal/length ([]) count should equal the length count ([]).", literalLengthCount, lengthCount);

				literalLengthDictionary.compute(literalLengthDistanceTable, HLIT);
				distanceDictionary.compute(literalLengthDistanceTable + HLIT, static_cast<uint16>(HDIST));

				while (true) {
					const uint16 literalLength = literalLengthDictionary.decode(stream);

					if (literalLength <= 255) {
						// Output literal.
						output.emplace_back(static_cast<uint8>(literalLength & 0xFF));

						continue;
					}

					if (literalLength == 256) {
						// End of stream.
						break;
					}

					ENGINE_ASSERT(literalLength <= 285, "Literal/length should not exceed 285.");

					const uint16 lengthExtraEntryIndex = literalLength - 257;
					const HuffmanCodeEntry& lengthExtraEntry = lengthExtra[lengthExtraEntryIndex];
					uint16 length = lengthExtraEntry.symbolIndex;

					if (lengthExtraEntry.bitLength > 0) {
						uint16 extraBits = stream.read<uint16>(lengthExtraEntry.bitLength);
						extraBits = HuffmanCodeDictionary::reverseBits(extraBits, lengthExtraEntry.bitLength);

						length += extraBits;
					}

					const uint16 distanceExtraEntryIndex = distanceDictionary.decode(stream);
					const HuffmanCodeEntry& distanceExtraEntry = distanceExtra[distanceExtraEntryIndex];
					uint16 distance = distanceExtraEntry.symbolIndex;

					if (distanceExtraEntry.bitLength > 0) {
						uint16 extraBits = stream.read<uint16>(distanceExtraEntry.bitLength);
						extraBits = HuffmanCodeDictionary::reverseBits(extraBits, distanceExtraEntry.bitLength);

						distance += extraBits;
					}

					const std::size_t previousSize = output.size();

					uint8* sourceIndex = &output[previousSize - 1] - distance;

					ENGINE_ASSERT(sourceIndex >= 0, "The source index is underflowing.");

					output.resize(previousSize + length);

					std::copy(sourceIndex, sourceIndex + length, &output[previousSize]);
				}

				break;
			}
			default: {
				throw std::runtime_error("Unhandled BTYPE value.");
			}
		}
	}

	void PNGFileLoader::readIEND(ByteStream& stream) {
		ENGINE_WARN("TODO: Handle IEND chunk.");
	}
}