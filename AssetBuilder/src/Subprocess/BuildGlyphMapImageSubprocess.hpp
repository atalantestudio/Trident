#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace ProjectA {
	class BuildGlyphMapImageSubprocess {
		private:
			static constexpr uint64 TIM_HEADER_SIZE = 9 * sizeof(uint32);

		public:
			// TODO: Mip level count is not handled at the moment and must be set to 1.
			explicit BuildGlyphMapImageSubprocess(const std::string& assetOutputPath, const std::string& assetName, const ref<ImageAsset>& imageAsset, uint32& assetIdentifier) {
				assetIdentifier = AssetLibrary::createAssetIdentifier();

				const uint8 channelCount = getChannelCount(imageAsset->getFormat());

				// NOTE: In the worst case, all chunks are QOI_RGBA.
				const uint64 maxOutputSize =
					TIM_HEADER_SIZE +
					imageAsset->getLayerCount() *
					imageAsset->getExtent().width *
					imageAsset->getExtent().height *
					imageAsset->getExtent().depth *
					(sizeof(uint8) + sizeof(qoi_rgba));

				ByteStream<StreamOperation::WRITE> outputByteStream(maxOutputSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::TIM);
				outputByteStream.write<uint32>(assetIdentifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(static_cast<uint32>(ImageFormat::R8G8B8A8_UNORM));
				outputByteStream.write<Extent3D>(imageAsset->getExtent());
				outputByteStream.write<uint32>(imageAsset->getMipLevelCount());
				outputByteStream.write<uint32>(imageAsset->getLayerCount());

				sequence<uint8> rgbaDataPerChannel(imageAsset->getData().count() * 4);

				convertRedToRGBA(imageAsset->getData(), rgbaDataPerChannel);

				const qoi_rgba* rgbaData = pointer_cast<const qoi_rgba*>(&rgbaDataPerChannel[0]);

				qoi_rgba texelCache[64]{};
				qoi_rgba previousTexel = {0, 0, 0, 255};
				qoi_index previousTexelHash = rgbaHash(previousTexel);
				uint8 runLength = 0;
				uint32 texelIndex = 0;

				for (uint32 layerIndex = 0; layerIndex < imageAsset->getLayerCount(); layerIndex += 1) {
					for (uint32 z = 0; z < imageAsset->getExtent().depth; z += 1) {
						for (uint32 y = 0; y < imageAsset->getExtent().height; y += 1) {
							for (uint32 x = 0; x < imageAsset->getExtent().width; x += 1, texelIndex += 1) {
								texelCache[previousTexelHash.index] = previousTexel;

								const qoi_rgba texel = rgbaData[texelIndex];
								const qoi_index texelHash = rgbaHash(texel);

								if (texelHash.index == previousTexelHash.index && runLength != 62) {
									runLength += 1;

									continue;
								}

								if (runLength > 0) {
									qoi_run run;
									run.tag = 0b11;
									run.run = runLength - 1;

									outputByteStream.write<qoi_run>(run);

									runLength = 0;
								}

								if (texel == texelCache[texelHash.index]) {
									qoi_index index;
									index.tag = 0b00;
									index.index = texelHash.index;

									outputByteStream.write<qoi_index>(index);
								}
								else {
									outputByteStream.write<uint8>(0b11111111);
									outputByteStream.write<qoi_rgba>(texel);
								}

								previousTexel = texel;
								previousTexelHash = texelHash;
							}
						}
					}
				}

				if (runLength > 0) {
					qoi_run run;
					run.tag = 0b11;
					run.run = runLength - 1;

					outputByteStream.write<qoi_run>(run);
				}

				const uint64 outputSize = outputByteStream.getByteOffset();

				const std::string outputPath = assetOutputPath + "Image/" + assetName + ".tim";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputSize);

				ENGINE_INFO("Generated glyph map image asset (ID: []).", assetIdentifier);
			}
	};
}