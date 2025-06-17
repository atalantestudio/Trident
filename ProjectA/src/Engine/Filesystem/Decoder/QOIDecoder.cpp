#include "pch.hpp"

#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

#pragma pack(push, 1)

namespace ProjectA {
	union rgba {
		struct {
			uint8 r;
			uint8 g;
			uint8 b;
			uint8 a;
		};

		uint32 value;
	};

	struct qoi_diff {
		uint8 db : 2;
		uint8 dg : 2;
		uint8 dr : 2;
		uint8 tag : 2;
	};

	struct qoi_luma {
		uint8 dg : 6;
		uint8 tag : 2;
		uint8 db_dg : 4;
		uint8 dr_dg : 4;
	};

	struct qoi_index {
		uint8 index : 6;
		uint8 tag : 2;
	};

	static qoi_index rgbaHash(rgba p) {
		qoi_index index;

		index.index = (p.r * 3 + p.g * 5 + p.b * 7 + p.a * 11) % 64;

		return index;
	}

	// https://qoiformat.org/qoi-specification.pdf
	template<>
	ImageFile Decoder::decode<FileFormat::QOI>(const sequence<uint8>& data) {
		ImageFile file;

		ByteStream stream(data, data.count);

		ENGINE_ASSERT(stream.read<uint32>() == STRING_TO_LITTLE_ENDIAN_UINT32("qoif"), "Invalid QOI file signature.");

		file.extent.width = stream.read<uint32>(true);
		file.extent.height = stream.read<uint32>(true);
		file.extent.depth = 1;

		const uint8 channelCount = stream.read<uint8>();

		switch (channelCount) {
			case 3:
				file.format = ImageFormat::R8G8B8_UNORM;

				break;
			case 4:
				file.format = ImageFormat::R8G8B8A8_UNORM;

				break;
			default:
				ENGINE_ASSERT(false, "Invalid channel count ([]).", static_cast<sint32>(channelCount));

				break;
		}

		const uint8 colorSpace = stream.read<uint8>();

		ENGINE_ASSERT(colorSpace == 0, "Unhandled color space ([]).", static_cast<sint32>(colorSpace));

		file.data = sequence<uint8>(file.extent.width * file.extent.height * channelCount);

		rgba texel = {0, 0, 0, 255};
		rgba previouslySeenTexels[64]{};

		uint64 dataOffset = 0;
		uint64 iterationByteOffset = 14;

		while (iterationByteOffset < data.count - 8) {
			uint8 tag = stream.peek<uint8>();

			if (tag == 0b11111110 || tag == 0b11111111) {
				// QOI_OP_RGB/QOI_OP_RGBA

				stream.jump(1);

				const rgba readTexel = stream.peek<rgba>();

				stream.jump(channelCount);

				texel.r = readTexel.r;
				texel.g = readTexel.g;
				texel.b = readTexel.b;

				if (channelCount == 4) {
					texel.a = readTexel.a;
				}

				std::copy_n(pointer_cast_const<const uint8*>(&texel), channelCount, &file.data[dataOffset]);

				dataOffset += channelCount;
				iterationByteOffset += channelCount + 1;
			}
			else {
				switch (tag >> 6) {
					case 0b00: {
						// QOI_OP_INDEX

						const qoi_index index = stream.read<qoi_index>();

						texel = previouslySeenTexels[index.index];

						std::copy_n(pointer_cast_const<const uint8*>(&texel), channelCount, &file.data[dataOffset]);

						dataOffset += channelCount;
						iterationByteOffset += 1;

						break;
					}
					case 0b01: {
						// QOI_OP_DIFF

						const qoi_diff diff = stream.read<qoi_diff>();

						texel.r += diff.dr - 2;
						texel.g += diff.dg - 2;
						texel.b += diff.db - 2;

						std::copy_n(pointer_cast_const<const uint8*>(&texel), channelCount, &file.data[dataOffset]);

						dataOffset += channelCount;
						iterationByteOffset += 1;

						break;
					}
					case 0b10: {
						// QOI_OP_LUMA

						const qoi_luma luma = stream.read<qoi_luma>();

						texel.r += luma.dg - 32 + luma.dr_dg - 8;
						texel.g += luma.dg - 32;
						texel.b += luma.dg - 32 + luma.db_dg - 8;

						std::copy_n(pointer_cast_const<const uint8*>(&texel), channelCount, &file.data[dataOffset]);

						dataOffset += channelCount;
						iterationByteOffset += 2;

						break;
					}
					case 0b11: {
						// QOI_OP_RUN

						stream.jump(1);

						const uint8 runLength = (tag & 0b00111111) + 1;

						for (uint8 runOffset = 0; runOffset < runLength; runOffset += 1) {
							std::copy_n(pointer_cast_const<const uint8*>(&texel), channelCount, &file.data[dataOffset]);

							dataOffset += channelCount;
						}

						iterationByteOffset += 1;

						break;
					}
				}
			}

			previouslySeenTexels[rgbaHash(texel).index] = texel;
		}

		return file;
	}
}

#pragma pack(pop)