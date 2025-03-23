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
		// TODO: Replace with memcmp or C++ equivalent.
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

		// TODO: Check CRC. See https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix.
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

		ENGINE_ASSERT(file.bitDepth == 8, "Bit depth = [] is unhandled.", file.bitDepth);
		ENGINE_ASSERT(file.colorType == 6, "Color type = [] is unhandled.", file.colorType);
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

	uint8 PNGFileLoader::applyAddFilter(const uint8* x, uint8* aOrB, uint8 channelIndex) {
		return (uint8) x[channelIndex] + (uint8) aOrB[channelIndex];
	}

	uint8 PNGFileLoader::applyAverageFilter(const uint8* x, uint8* a, uint8* b, uint8 channelIndex) {
		const uint32 average = ((uint32) a[channelIndex] + (uint32) b[channelIndex]) / 2;

		return (uint8) x[channelIndex] + (uint8) average;
	}

	// TODO: Use vec3<sint32>.
	uint8 PNGFileLoader::applyPaethFilter(const uint8* x, uint8* aPixel, uint8* bPixel, uint8* cPixel, uint8 channelIndex) {
		const sint32 a = (sint32) aPixel[channelIndex];
		const sint32 b = (sint32) bPixel[channelIndex];
		const sint32 c = (sint32) cPixel[channelIndex];

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

		return (uint8) x[channelIndex] + (uint8) paeth;
	}
}