#include "pch.hpp"

#include "Engine/Compression/HuffmanCodeDictionary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	enum class FilterType : uint8 {
		NONE = 0,
		SUB = 1,
		UP = 2,
		AVERAGE = 3,
		PAETH = 4,
	};

	/* static void readIHDRChunk(ByteStream& stream, File<FileFormat::PNG>& file) {
		file.extent.width = stream.read<uint32>(true);

		ENGINE_ASSERT(file.extent.width > 0, "The width must be > 0.");

		file.extent.height = stream.read<uint32>(true);

		ENGINE_ASSERT(file.extent.height > 0, "The height must be > 0.");

		file.extent.depth = 1;

		file.bitDepth = stream.read<uint8>();

		ENGINE_ASSERT(
			file.bitDepth == 1 ||
			file.bitDepth == 2 ||
			file.bitDepth == 4 ||
			file.bitDepth == 8 ||
			file.bitDepth == 16,
			"Undefined bit depth ([]).", static_cast<sint32>(file.bitDepth)
		);

		file.colorType = stream.read<uint8>();

		ENGINE_ASSERT(
			file.colorType == 0 ||
			file.colorType == 2 ||
			file.colorType == 3 ||
			file.colorType == 4 ||
			file.colorType == 6,
			"Undefined color type ([]).", static_cast<sint32>(file.colorType)
		);

		file.compressionMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.compressionMethod == 0, "Undefined compression method ([]).", static_cast<sint32>(file.compressionMethod));

		file.filterMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.filterMethod == 0, "Undefined filter method ([]).", static_cast<sint32>(file.filterMethod));

		file.interlaceMethod = stream.read<uint8>();

		ENGINE_ASSERT(file.interlaceMethod == 0 || file.interlaceMethod == 1, "Undefined interlace method ([]).", static_cast<sint32>(file.interlaceMethod));

		// Assertions for values currently unhandled by the engine.
		ENGINE_ASSERT(file.bitDepth == 8, "Unhandled bit depth ([]).", static_cast<sint32>(file.bitDepth));
		ENGINE_ASSERT(file.colorType == 0 || file.colorType == 2 || file.colorType == 6, "Unhandled color type ([]).", static_cast<sint32>(file.colorType));
	} */

	/* static void readIDATChunk(ByteStream& stream, uint32 chunkByteLength, std::vector<uint8>& compressedData) {
		compressedData.resize(compressedData.size() + chunkByteLength);

		std::copy(
			stream.getBytePosition(),
			stream.getBytePosition() + chunkByteLength,
			compressedData.end() - chunkByteLength
		);

		// TODO: Skip chunk data for the moment.
		stream.jump(chunkByteLength);
	} */

	/* static void readChunk(ByteStream& stream, File<FileFormat::PNG>& file, std::vector<uint8>& compressedData) {
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
		// const uint32 CRC = stream.read<uint32>(true);
		stream.jump(sizeof(uint32));
	} */

	static uint8 applyAverageFilter(const uint8* x, uint8* a, uint8* b, uint8 channelIndex) {
		const uint32 average = ((uint32) a[channelIndex] + (uint32) b[channelIndex]) / 2;

		return x[channelIndex] + static_cast<uint8>(average);
	}

	// TODO: Use vec3<sint32>.
	static uint8 applyPaethFilter(const uint8* x, uint8* aPixel, uint8* bPixel, uint8* cPixel, uint8 channelIndex) {
		const sint32 a = static_cast<sint32>(aPixel[channelIndex]);
		const sint32 b = static_cast<sint32>(bPixel[channelIndex]);
		const sint32 c = static_cast<sint32>(cPixel[channelIndex]);
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

		return x[channelIndex] + static_cast<uint8>(paeth);
	}

	/* template<>
	File<FileFormat::PNG> Filesystem::decode(const Stream& stream) {
		File<FileFormat::PNG> file;
		ByteStream byteStream = stream.asByteStream();

		ENGINE_ASSERT(byteStream.read<uint64>(true) == 0x89504E470D0A1A0A, "Invalid PNG file signature.");

		std::vector<uint8> compressedData;

		while (byteStream.getRemainingByteSize() > 0) {
			readChunk(byteStream, file, compressedData);
		}

		uint8 channelCount;

		switch (file.colorType) {
			case 0:
				channelCount = 1;

				file.format = ImageFormat::R8_UNORM;

				break;
			case 2:
				channelCount = 3;

				file.format = ImageFormat::R8G8B8_UNORM;

				break;
			case 6:
				channelCount = 4;

				file.format = ImageFormat::R8G8B8A8_UNORM;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}

		BitStream bitStream(compressedData.data(), compressedData.size());

		const uint32 uncompressedSize = file.extent.width * file.extent.height * channelCount + (file.extent.height * 1);

		const sequence<uint8> unfilteredData = bitStream.inflate(uncompressedSize);

		file.data = sequence<uint8>(file.extent.width * file.extent.height * channelCount);

		const uint8* source = unfilteredData;
		uint8* destination = &file.data[0];

		constexpr uint8 ZERO = 0;

		uint8* previousRow = const_cast<uint8*>(&ZERO);
		sint32 previousRowAdvance = 0;

		// TODO: Refactor with SIMD.
		for (uint32 y = 0; y < file.extent.height; y += 1) {
			const FilterType filterType = static_cast<FilterType>(*source);

			source += 1;

			uint8* currentRow = destination;

			switch (filterType) {
				case FilterType::NONE: {
					std::copy(source, source + file.extent.width * channelCount, destination);

					source += file.extent.width * channelCount;
					destination += file.extent.width * channelCount;

					break;
				}
				case FilterType::SUB: {
					uint32 aPixel = 0;

					for (uint32 x = 0; x < file.extent.width; x += 1) {
						for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
							destination[channelIndex] = source[channelIndex] + pointer_cast<uint8*>(&aPixel)[channelIndex];
						}

						aPixel = *pointer_cast_const<const uint32*>(destination);
						destination += channelCount;
						source += channelCount;
					}

					break;
				}
				case FilterType::UP: {
					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < file.extent.width; x += 1) {
						for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
							destination[channelIndex] = source[channelIndex] + bPixel[channelIndex];
						}

						bPixel += previousRowAdvance;
						destination += channelCount;
						source += channelCount;
					}

					break;
				}
				case FilterType::AVERAGE: {
					uint32 aPixel = 0;
					uint8* bPixel = previousRow;

					for (uint32 x = 0; x < file.extent.width; x += 1) {
						for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
							destination[channelIndex] = applyAverageFilter(source, pointer_cast<uint8*>(&aPixel), bPixel, channelIndex);
						}

						aPixel = *pointer_cast_const<const uint32*>(destination);
						bPixel += previousRowAdvance;
						destination += channelCount;
						source += channelCount;
					}

					break;
				}
				case FilterType::PAETH: {
					uint32 aPixel = 0;
					uint8* bPixel = previousRow;
					uint32 cPixel = 0;

					for (uint32 x = 0; x < file.extent.width; x += 1) {
						for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
							destination[channelIndex] = applyPaethFilter(source, pointer_cast<uint8*>(&aPixel), bPixel, pointer_cast<uint8*>(&cPixel), channelIndex);
						}

						aPixel = *pointer_cast_const<const uint32*>(destination);
						cPixel = *pointer_cast_const<const uint32*>(bPixel);
						bPixel += previousRowAdvance;
						destination += channelCount;
						source += channelCount;
					}

					break;
				}
				default:
					ENGINE_ERROR("Unhandled filter type [].", static_cast<sint32>(filterType));
					DEBUG_ASSERT(false);

					break;
			}

			previousRowAdvance = channelCount;
			previousRow = currentRow;
		}

		return file;
	} */
}