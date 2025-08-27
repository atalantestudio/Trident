#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	template<>
	File<FileFormat::QOI> Filesystem::decode<FileFormat::QOI>(const sequence<uint8>& data) {
		File<FileFormat::QOI> file;

		ByteStream<StreamOperation::READ> stream(&data[0], data.count, Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.read<uint32>(true) == STRING_TO_BIG_ENDIAN_UINT32("qoif"), "Invalid QOI file signature.");

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
				ENGINE_ASSERT(false, "Unhandled channel count ([]).", static_cast<sint32>(channelCount));

				break;
		}

		const uint8 colorSpace = stream.read<uint8>();

		ENGINE_ASSERT(colorSpace == 0, "Unhandled color space ([]).", static_cast<sint32>(colorSpace));

		file.data = sequence<uint8>(file.extent.width * file.extent.height * channelCount);

		// We cannot parse more than the total byte size - (14-byte header + 8-byte footer).
		// NOTE: This implementation doesn't look at the footer.
		const uint64 maxIterationCount = data.count - 22;

		QOIDecoderContentOnly(stream, file.data, channelCount, maxIterationCount);

		return file;
	}

	void Filesystem::QOIDecoderContentOnly(ByteStream<StreamOperation::READ>& stream, sequence<uint8>& data, uint8 channelCount, uint64 maxIterationCount) {
		qoi_rgba previousTexel = {0, 0, 0, 255};
		qoi_rgba texelCache[64]{};

		uint64 dataOffset = 0;
		uint64 iterationIndex = 0;

		while (iterationIndex < maxIterationCount) {
			const uint8 tag = stream.peek<uint8>();

			if (tag >= 0b11111110) {
				// QOI_OP_RGB/QOI_OP_RGBA

				stream.jump(1);

				const qoi_rgba texel = stream.peek<qoi_rgba>();

				previousTexel.r = texel.r;
				previousTexel.g = texel.g;
				previousTexel.b = texel.b;

				stream.jump(tag - 251);

				if (tag == 0b11111111) {
					previousTexel.a = texel.a;
				}

				std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

				dataOffset += channelCount;
				iterationIndex += channelCount + 1;
			}
			else {
				switch (tag >> 6) {
					case 0b00: {
						// QOI_OP_INDEX

						const qoi_index index = stream.read<qoi_index>();

						previousTexel = texelCache[index.index];

						std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

						dataOffset += channelCount;
						iterationIndex += 1;

						break;
					}
					case 0b01: {
						// QOI_OP_DIFF

						const qoi_diff diff = stream.read<qoi_diff>();

						previousTexel.r += diff.dr - 2;
						previousTexel.g += diff.dg - 2;
						previousTexel.b += diff.db - 2;

						std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

						dataOffset += channelCount;
						iterationIndex += 1;

						break;
					}
					case 0b10: {
						// QOI_OP_LUMA

						const qoi_luma luma = stream.read<qoi_luma>();

						previousTexel.r += luma.dg - 32 + luma.dr_dg - 8;
						previousTexel.g += luma.dg - 32;
						previousTexel.b += luma.dg - 32 + luma.db_dg - 8;

						std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

						dataOffset += channelCount;
						iterationIndex += 2;

						break;
					}
					case 0b11: {
						// QOI_OP_RUN

						stream.jump(1);

						const uint8 runLength = (tag & 0b00111111) + 1;

						for (uint8 runOffset = 0; runOffset < runLength; runOffset += 1) {
							std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

							dataOffset += channelCount;
						}

						iterationIndex += 1;

						break;
					}
				}
			}

			texelCache[rgbaHash(previousTexel).index] = previousTexel;
		}
	}
}