#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	static sint32 PaethPredictor(uint8 a, uint8 b, uint8 c) {
		const sint16 p = a + b - c;
		const uint16 pa = abs(p - a);
		const uint16 pb = abs(p - b);
		const uint16 pc = abs(p - c);

		if (pa <= pb && pa <= pc) {
			return a;
		}

		if (pb <= pc) {
			return b;
		}

		return c;
	}

	typedef uint8 (*Filter)(uint8 x, uint8 a, uint8 b, uint8 c);

	static uint8 None(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x;
	}

	static uint8 Sub(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x + a;
	}

	static uint8 Up(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x + b;
	}

	// https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-type-3-Average
	static uint8 Average(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x + (a + b) / 2;
	}

	// https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-type-4-Paeth
	static uint8 Paeth(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x + PaethPredictor(a, b, c);
	}

	static constexpr uint32 ZERO_PIXEL = 0;

	static constexpr Filter FILTERS[] = {None, Sub, Up, Average, Paeth};

	template<>
	File<FileFormat::PNG> Filesystem::decode(const sequence<uint8>& data) {
		File<FileFormat::PNG> file;

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::BIG_ENDIAN);

		const uint64 signature = stream.read<uint64>();

		ENGINE_ASSERT(signature == 0x89504E470D0A1A0A, "Invalid PNG file signature.");

		uint64 compressedDataSize = 0;
		uint8 channelCount = 0;

		while (stream.getRemainingSize() > 0) {
			const uint32 byteLength = stream.read<uint32>();
			const uint32 type = stream.read<uint32>(true);

			switch (type) {
				case STRING_TO_LITTLE_ENDIAN_UINT32("IHDR"): {
					file.extent.width = stream.read<uint32>();

					ENGINE_ASSERT(file.extent.width > 0, "The width must be > 0.");

					file.extent.height = stream.read<uint32>();

					ENGINE_ASSERT(file.extent.height > 0, "The height must be > 0.");

					file.extent.depth = 1;

					// Number of bytes per sample or per palette index (not per pixel).
					// Valid values are 1, 2, 4, 8, and 16, although not all values are allowed for all color types.
					const uint8 bitDepth = stream.read<uint8>();

					ENGINE_ASSERT(
						bitDepth == 1 ||
						bitDepth == 2 ||
						bitDepth == 4 ||
						bitDepth == 8 ||
						bitDepth == 16,
						"Undefined bit depth ([]).", static_cast<sint32>(bitDepth)
					);
					ENGINE_ASSERT(bitDepth == 8, "Unsupported bit depth ([]).", static_cast<sint32>(bitDepth));

					// PNG image type.
					// Valid values are 0, 2, 3, 4, and 6.
					const uint8 colorType = stream.read<uint8>();

					ENGINE_ASSERT(
						colorType == 0 ||
						colorType == 2 ||
						colorType == 3 ||
						colorType == 4 ||
						colorType == 6,
						"Undefined color type ([]).", static_cast<sint32>(colorType)
					);
					ENGINE_ASSERT(colorType == 0 || colorType == 2 || colorType == 6, "Unsupported color type ([]).", static_cast<sint32>(colorType));

					switch (colorType) {
						case 0:
							channelCount = 1;

							break;
						case 2:
							channelCount = 3;

							break;
						case 6:
							channelCount = 4;

							break;
					}

					file.format = findUnormFormat(channelCount);

					// Method used to compress the image data.
					// Only compression method 0 (deflate/inflate compression with a sliding window of at most 32768 bytes) is defined.
					const uint8 compressionMethod = stream.read<uint8>();

					ENGINE_ASSERT(compressionMethod == 0, "Undefined compression method ([]).", static_cast<sint32>(compressionMethod));

					// Preprocessing method applied to the image data before compression.
					// Only filter method 0 (adaptive filtering with five basic filter types) is defined.
					const uint8 filterMethod = stream.read<uint8>();

					ENGINE_ASSERT(filterMethod == 0, "Undefined filter method ([]).", static_cast<sint32>(filterMethod));

					// Transmission order of the image data.
					// Two values are defined: 0 (no interlace) or 1 (Adam7 interlace).
					const uint8 interlaceMethod = stream.read<uint8>();

					ENGINE_ASSERT(interlaceMethod == 0 || interlaceMethod == 1, "Undefined interlace method ([]).", static_cast<sint32>(interlaceMethod));
					ENGINE_ASSERT(interlaceMethod == 0, "Unsupported interlace method ([]).", static_cast<sint32>(interlaceMethod));

					break;
				}
				case STRING_TO_LITTLE_ENDIAN_UINT32("IDAT"): {
					compressedDataSize += byteLength;

					stream.jump(byteLength);

					break;
				}
				case STRING_TO_LITTLE_ENDIAN_UINT32("IEND"):
					break;
				default: {
					const char8* typeString = pointer_cast<const char8*>(&type);

					ENGINE_WARNING("Ignoring unsupported PNG chunk \"[][][][]\".", typeString[0], typeString[1], typeString[2], typeString[3]);

					stream.jump(byteLength);

					break;
				}
			}

			const uint32 CRC = stream.read<uint32>();

			// TODO: Check CRC. See https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix.
		}

		sequence<uint8> compressedData(compressedDataSize);
		uint64 compressedDataOffset = 0;

		stream.seek(sizeof(uint64));

		while (stream.getRemainingSize() > 0) {
			const uint32 byteLength = stream.read<uint32>();
			const uint32 type = stream.read<uint32>(true);

			if (type == STRING_TO_LITTLE_ENDIAN_UINT32("IDAT")) {
				std::copy_n(stream.getBytePosition(), byteLength, &compressedData[compressedDataOffset]);

				compressedDataOffset += byteLength;
			}

			// Ignore CRC.
			stream.jump(byteLength + sizeof(uint32));
		}

		BitStream<StreamOperation::READ> inflateBitStream(&compressedData[0], compressedDataSize, Endianness::LITTLE_ENDIAN);

		const uint64 filteredDataSize = file.extent.width * file.extent.height * channelCount;
		const uint64 unfilteredDataSize = filteredDataSize + file.extent.height;

		const sequence<uint8> unfilteredData = inflateBitStream.inflate<uint8>(unfilteredDataSize);

		file.data = sequence<uint8>(filteredDataSize);

		uint8* destination = &file.data[0];
		uint32 bAdvance = 0;

		// Current pixel.
		const uint8* x = &unfilteredData[0];

		// Pixel at the left of `x`. If `x` is the leftmost pixel, `a` is 0.
		const uint8* a;

		// Pixel above `x`. If `x` is the upmost pixel, `b` is 0.
		const uint8* b = pointer_cast<const uint8*>(&ZERO_PIXEL);

		// Pixel at the left of `b`. If `b` is the leftmost pixel, `c` is 0.
		const uint8* c;

		// TODO: Optimize with SIMD when the image has 4 channels.
		for (uint32 rowIndex = 0; rowIndex < file.extent.height; rowIndex += 1) {
			const uint8 filterType = *x;

			ENGINE_ASSERT(filterType < 5, "Undefined filter type ([]).", static_cast<sint32>(filterType));

			x += 1;
			a = pointer_cast<const uint8*>(&ZERO_PIXEL);
			c = pointer_cast<const uint8*>(&ZERO_PIXEL);

			const uint8* row = destination;

			for (uint32 columnIndex = 0; columnIndex < file.extent.width; columnIndex += 1) {
				for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
					destination[channelIndex] = FILTERS[filterType](x[channelIndex], a[channelIndex], b[channelIndex], c[channelIndex]);
				}

				c = b;
				b += bAdvance * channelCount;
				a = destination;
				x += channelCount;
				destination += channelCount;
			}

			b = row;
			bAdvance = 1;
		}

		return file;
	}
}