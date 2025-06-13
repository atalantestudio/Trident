#include "pch.hpp"

#include "Engine/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/File/File.hpp"
#include "Engine/Filesystem/File/Format/PNG/FilterType.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	static void readIHDRChunk(ByteStream& stream, PNGFile& file) {
		file.width = stream.read<uint32>(true);

		ENGINE_ASSERT(file.width > 0, "The width must be > 0.");

		file.height = stream.read<uint32>(true);

		ENGINE_ASSERT(file.height > 0, "The height must be > 0.");

		file.bitDepth = stream.read<uint8>();

		ENGINE_ASSERT(
			file.bitDepth == 1 ||
			file.bitDepth == 2 ||
			file.bitDepth == 4 ||
			file.bitDepth == 8 ||
			file.bitDepth == 16,
			"Invalid bit depth."
		);

		file.colorType = stream.read<uint8>();

		ENGINE_ASSERT(
			file.colorType == 0 ||
			file.colorType == 2 ||
			file.colorType == 3 ||
			file.colorType == 4 ||
			file.colorType == 6,
			"Invalid color type."
		);

		file.compressionMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.compressionMethod == 0, "Undefined compression method.");

		file.filterMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.filterMethod == 0, "Undefined filter method.");

		file.interlaceMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.interlaceMethod == 0 || file.interlaceMethod == 1, "Undefined interlace method.");

		// Assertions for currently unhandled values.
		ENGINE_ASSERT(file.bitDepth == 8, "Bit depth = [] is unhandled.", static_cast<sint32>(file.bitDepth));
		ENGINE_ASSERT(file.colorType == 6, "Color type = [] is unhandled.", static_cast<sint32>(file.colorType));
	}

	static void readIDATChunk(ByteStream& stream, uint32 chunkByteLength, std::vector<uint8>& compressedData) {
		compressedData.resize(compressedData.size() + chunkByteLength);

		std::copy(
			stream.getBytePosition(),
			stream.getBytePosition() + chunkByteLength,
			compressedData.end() - chunkByteLength
		);

		// TODO: Skip chunk data for the moment.
		stream.jump(chunkByteLength);
	}

	static void readChunk(ByteStream& stream, PNGFile& file, std::vector<uint8>& compressedData) {
		const uint32 ByteLength = stream.read<uint32>(true);
		const uint32 Type = stream.read<uint32>();

		switch (Type) {
			case STRING_TO_LITTLE_ENDIAN_UINT32("IHDR"):
				readIHDRChunk(stream, file);

				break;
			case STRING_TO_LITTLE_ENDIAN_UINT32("IDAT"):
				readIDATChunk(stream, ByteLength, compressedData);

				break;
			case STRING_TO_LITTLE_ENDIAN_UINT32("IEND"):
				break;
			default:
				const char* TypeString = static_cast<const char*>(static_cast<const void*>(&Type));

				ENGINE_WARN("Ignored unhandled PNG chunk type \"[][][][]\".", TypeString[0], TypeString[1], TypeString[2], TypeString[3]);

				stream.jump(ByteLength);

				break;
		}

		// TODO: Check CRC. See https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix.
		const uint32 CRC = stream.read<uint32>(true);
	}

	static uint8 applyAddFilter(const uint8* x, uint8* aOrB, uint8 channelIndex) {
		return (uint8) x[channelIndex] + (uint8) aOrB[channelIndex];
	}

	static uint8 applyAverageFilter(const uint8* x, uint8* a, uint8* b, uint8 channelIndex) {
		const uint32 average = ((uint32) a[channelIndex] + (uint32) b[channelIndex]) / 2;

		return (uint8) x[channelIndex] + (uint8) average;
	}

	// TODO: Use vec3<sint32>.
	static uint8 applyPaethFilter(const uint8* x, uint8* aPixel, uint8* bPixel, uint8* cPixel, uint8 channelIndex) {
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

	template<>
	PNGFile Filesystem::openTemporary(std::string_view path) {
		const File baseFile = openTemporary<File>(path);

		ByteStream stream(baseFile.rawData, baseFile.size);

		ENGINE_ASSERT(stream.read<uint64>(true) == 0x89504E470D0A1A0A, "Failed to verify signature.");

		PNGFile file;

		std::vector<uint8> compressedData;

		while (stream.getRemainingByteSize() > 0) {
			readChunk(stream, file, compressedData);
		}

		BitStream bitStream(compressedData.data(), compressedData.size());

		const uint32 finalSize = file.width * file.height * 4 + (file.height * 1);

		const sequence<uint8> unfilteredData = bitStream.inflate(finalSize);

		file.data = sequence<uint8>(file.width * file.height * 4);

		const uint8* source = unfilteredData;
		uint8* destination = &file.data[0];

		const uint32 rowLength = file.width * 4 + 1;

		const uint8 zero = 0;
		uint8* previousRow = (uint8*) &zero;
		sint32 previousRowAdvance = 0;

		// TODO: Refactor with SIMD.
		for (uint32 y = 0; y < file.height; y += 1) {
			const FilterType filterType = static_cast<FilterType>(*source);

			source += 1;

			uint8* currentRow = destination;

			switch (filterType) {
				case FilterType::NONE: {
					std::copy(source, source + file.width * 4, destination);

					source += file.width * 4;
					destination += file.width * 4;

					break;
				}
				case FilterType::SUB: {
					uint32 aPixel = 0;

					for (uint32 x = 0; x < file.width; x += 1) {
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
					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < file.width; x += 1) {
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
					uint32 aPixel = 0;
					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < file.width; x += 1) {
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
					uint32 aPixel = 0;
					uint8* bPixel = previousRow;
					uint32 cPixel = 0;

					for (uint32 x = 0; x < file.width; x += 1) {
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

		return file;
	}
}