// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/File/QOI.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	bool Decoder<FileFormat::QOI>::validateSignature(const ByteStream<StreamOperation::READ>& stream) {
		if (stream.getRemainingSize() < sizeof(uint32)) {
			return false;
		}

		const uint32 magic = stream.peek<uint32>(true);

		return magic == STRING_TO_BIG_ENDIAN_UINT32("qoif");
	}

	template<>
	File<FileFormat::QOI> Decoder<FileFormat::QOI>::decode(view<uint8> data) {
		constexpr uint64 QOI_FOOTER = 0x100000000000000;

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(Decoder<FileFormat::QOI>::validateSignature(stream), "Invalid QOI signature.");

		stream.jump(sizeof(uint32));

		File<FileFormat::QOI> file{};
		file.extent.width = stream.read<uint32>(true);
		file.extent.height = stream.read<uint32>(true);
		file.extent.depth = 1;

		const uint8 channelCount = stream.read<uint8>();

		switch (channelCount) {
			case 3: {
				file.format = ImageFormat::R8G8B8_UNORM;

				break;
			}
			case 4: {
				file.format = ImageFormat::R8G8B8A8_UNORM;

				break;
			}
			default: {
				ENGINE_ASSERT(false, "Unhandled channel count ([]).", static_cast<sint32>(channelCount));

				break;
			}
		}

		const uint8 colorSpace = stream.read<uint8>();

		ENGINE_ASSERT(colorSpace == 0, "Unhandled color space ([]).", static_cast<sint32>(colorSpace));

		file.data = sequence<uint8>(file.extent.width * file.extent.height * channelCount);

		Filesystem::decodeQoiContent(stream, file.data, channelCount, 8);

		const uint8* footer = stream.getBytePosition();

		ENGINE_ASSERT(std::memcmp(footer, &QOI_FOOTER, sizeof(uint64)) == 0, "QOI footer contains invalid bytes.");

		return file;
	}

	void Filesystem::decodeQoiContent(ByteStream<StreamOperation::READ>& stream, sequence<uint8>& data, uint8 channelCount, uint32 footerByteSize) {
		qoi_rgba previousTexel = {0, 0, 0, 255};
		qoi_rgba texelCache[64]{};
		uint64 dataOffset = 0;

		while (stream.getRemainingSize() > footerByteSize) {
			const uint8 tag = stream.peek<uint8>();

			if (tag >= 0b11111110) {
				// QOI_OP_RGB/QOI_OP_RGBA
				stream.jump(1);

				if (tag == 0b11111111) {
					const qoi_rgba texel = stream.read<qoi_rgba>();

					previousTexel = texel;
				}
				else {
					const qoi_rgb texel = stream.read<qoi_rgb>();

					previousTexel.r = texel.r;
					previousTexel.g = texel.g;
					previousTexel.b = texel.b;
				}

				std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

				dataOffset += channelCount;
			}
			else {
				switch (tag >> 6) {
					case 0b00: {
						// QOI_OP_INDEX
						const qoi_index index = stream.read<qoi_index>();

						previousTexel = texelCache[index.index];

						std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

						dataOffset += channelCount;

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

						break;
					}
					case 0b11: {
						// QOI_OP_RUN
						const qoi_run run = stream.read<qoi_run>();
						const uint8 runLength = run.run + 1;

						uint8 runOffset = 0;

						while (runOffset < runLength) {
							std::copy_n(pointer_cast<const uint8*>(&previousTexel), channelCount, &data[dataOffset]);

							dataOffset += channelCount;
							runOffset += 1;
						}

						break;
					}
				}
			}

			texelCache[rgbaHash(previousTexel).index] = previousTexel;
		}
	}
}