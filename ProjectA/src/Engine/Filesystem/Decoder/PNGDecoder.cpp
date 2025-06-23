#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	static sint32 PaethPredictor(uint8 a, uint8 b, uint8 c) {
		/* const uint8 p = a + b - c;
		const uint8 pa = abs(p - a);
		const uint8 pb = abs(p - b);
		const uint8 pc = abs(p - c);

		if (pa <= pb && pa <= pc) {
			return a;
		}

		if (pb <= pc) {
			return b;
		}

		return c; */
		const sint32 _a = static_cast<sint32>(a);
		const sint32 _b = static_cast<sint32>(b);
		const sint32 _c = static_cast<sint32>(c);
		const sint32 p = _a + _b - _c;

		sint32 pa = p - _a;
		sint32 pb = p - _b;
		sint32 pc = p - _c;

		if (pa < 0) pa = -pa;
		if (pb < 0) pb = -pb;
		if (pc < 0) pc = -pc;

		if (pa <= pb && pa <= pc) {
			return _a;
		}

		if (pb <= pc) {
			return _b;
		}

		return _c;
	}

	typedef uint8 (*Filter)(uint8 x, uint8 a, uint8 b, uint8 c);

	static uint8 None(uint8 x, uint8 a, uint8 b, uint8 c) {
		return x;
	}

	static uint8 Sub(uint8 x, uint8 a, uint8 b, uint8 c) {
		const uint16 sum = x + a;

		// BREAKPOINT();

		// return x + a;
		return static_cast<uint8>(sum % 256);
	}

	static uint8 Up(uint8 x, uint8 a, uint8 b, uint8 c) {
		const uint16 sum = x + b;

		// BREAKPOINT();

		// return x + b;
		return static_cast<uint8>(sum % 256);
	}

	// https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-type-3-Average
	static uint8 Average(uint8 x, uint8 a, uint8 b, uint8 c) {
		// BREAKPOINT();

		const uint16 sum = a + b;

		return static_cast<uint8>((x + sum / 2) % 256);
	}

	// https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-type-4-Paeth
	static uint8 Paeth(uint8 x, uint8 a, uint8 b, uint8 c) {
		const uint32 sum = x + PaethPredictor(a, b, c);

		// BREAKPOINT();

		return static_cast<uint8>(sum % 256);
	}

	static constexpr uint32 ZERO_PIXEL = 0;

	static constexpr Filter FILTERS[] = {None, Sub, Up, Average, Paeth};

	template<>
	File<FileFormat::PNG> Filesystem::decode(const Stream& baseStream) {
		File<FileFormat::PNG> file;

		ByteStream stream = baseStream.asByteStream();

		const uint64 signature = stream.read<uint64>(true);

		ENGINE_ASSERT(signature == 0x89504E470D0A1A0A, "Invalid PNG file signature.");

		uint64 compressedDataSize = 0;
		uint8 channelCount = 0;

		while (stream.getRemainingByteSize() > 0) {
			const uint32 byteLength = stream.read<uint32>(true);
			const uint32 type = stream.read<uint32>();

			switch (type) {
				case STRING_TO_LITTLE_ENDIAN_UINT32("IHDR"): {
					file.extent.width = stream.read<uint32>(true);

					ENGINE_ASSERT(file.extent.width > 0, "The width must be > 0.");

					file.extent.height = stream.read<uint32>(true);

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
					const char* typeString = pointer_cast_const<const char*>(&type);

					ENGINE_WARN("Ignoring unsupported PNG chunk of type \"[][][][]\".", typeString[0], typeString[1], typeString[2], typeString[3]);

					stream.jump(byteLength);

					break;
				}
			}

			const uint32 CRC = stream.read<uint32>(true);

			// TODO: Check CRC. See https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix.
		}

		sequence<uint8> compressedData(compressedDataSize);
		uint64 compressedDataOffset = 0;

		stream.seek(sizeof(uint64));

		while (stream.getRemainingByteSize() > 0) {
			const uint32 byteLength = stream.read<uint32>(true);
			const uint32 type = stream.read<uint32>();

			if (type == STRING_TO_LITTLE_ENDIAN_UINT32("IDAT")) {
				std::copy_n(stream.getBytePosition(), byteLength, &compressedData[compressedDataOffset]);

				compressedDataOffset += byteLength;
			}

			// Ignore CRC.
			stream.jump(byteLength + sizeof(uint32));
		}

		BitStream inflateStream(compressedData, compressedDataSize);

		const uint64 filteredDataSize = file.extent.width * file.extent.height * channelCount;
		const uint64 unfilteredDataSize = filteredDataSize + file.extent.height;

		const sequence<uint8> unfilteredData = inflateStream.inflate(unfilteredDataSize);

		file.data = sequence<uint8>(filteredDataSize);

		uint8* destination = &file.data[0];
		const uint8* previousRow = pointer_cast_const<const uint8*>(&ZERO_PIXEL);
		uint32 bAdvance = 0;

		/* for (uint32 i = 0; i < file.extent.height; i += 1) {
			for (uint32 j = 1; j < file.extent.width; j += 4) {
				ENGINE_DEBUG("[] [] [] []", (int) unfilteredData[i * file.extent.height + j + 0], (int) unfilteredData[i * file.extent.height + j + 1], (int) unfilteredData[i * file.extent.height + j + 2], (int) unfilteredData[i * file.extent.height + j + 3]);
			}
		} */

		// The byte being filtered.
		const uint8* x = unfilteredData;

		// The byte corresponding to x in the pixel immediately before the pixel containing x.
		// const uint8* a;

		// The byte corresponding to x in the previous scanline.
		// const uint8* b = pointer_cast_const<const uint8*>(&ZERO_PIXEL);

		// The byte corresponding to b in the pixel immediately before the pixel containing b.
		// const uint8* c;

		// TODO: Use SIMD when the image has 4 channels.
		for (uint32 rowIndex = 0; rowIndex < file.extent.height; rowIndex += 1) {
			const uint8 filterType = *x;

			ENGINE_ASSERT(filterType < 5, "Undefined filter type ([]).", static_cast<sint32>(filterType));

			x += 1;

			const uint8* row = destination;

			for (uint32 columnIndex = 0; columnIndex < file.extent.width; columnIndex += 1) {
				const uint8* a = pointer_cast_const<const uint8*>(&ZERO_PIXEL);
				const uint8* b = row;
				const uint8* c = pointer_cast_const<const uint8*>(&ZERO_PIXEL);
				// if (rowIndex == 22 && columnIndex == 18) BREAKPOINT();

				for (uint8 channelIndex = 0; channelIndex < channelCount; channelIndex += 1) {
					destination[channelIndex] = FILTERS[filterType](*x, *a, *b, *c);

					c += 1;
					b += bAdvance;
					a += 1;
					x += 1;
				}

				c = b;
				a = destination;

				destination += channelCount;
			}

			previousRow = row;
			bAdvance = 1;

			if (rowIndex == 22) {
				// BREAKPOINT();

				// break;
			}
		}

		return file;
	}
}