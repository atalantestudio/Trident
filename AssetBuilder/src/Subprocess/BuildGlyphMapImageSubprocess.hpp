#pragma once

// TODO: Remove this include.
#include "ProjectA.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Decoder/QOIDecoder.cpp"

namespace ProjectA {
	class BuildGlyphMapImageSubprocess {
		private:
			struct rgba {
				uint8 r;
				uint8 g;
				uint8 b;
				uint8 a;
			};

		private:
			static constexpr uint64 TIM_HEADER_SIZE = 8 * sizeof(uint32);

		public:
			explicit BuildGlyphMapImageSubprocess(const std::string& assetOutputPath, const std::string& assetName, const ref<ImageAsset>& imageAsset, uint32& assetIdentifier) {
				assetIdentifier = AssetLibrary::createAssetIdentifier();

				const uint64 maxOutputSize = TIM_HEADER_SIZE + imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * sizeof(uint32);

				ByteStream<StreamOperation::WRITE> outputByteStream(maxOutputSize);
				outputByteStream.write<uint32>(AssetSignature::TIM);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(static_cast<uint32>(imageAsset->getFormat()));
				outputByteStream.write<Extent3D>(imageAsset->getExtent());
				outputByteStream.write<uint32>(imageAsset->getMipLevelCount());
				outputByteStream.write<uint32>(imageAsset->getLayerCount());

				const uint64 texelCount = imageAsset->getData().count;

				sequence<uint8> rgbaData(texelCount * 4);

				// Convert R to RGBA.
				for (uint32 texelIndex = 0, channelIndex = 0; texelIndex < texelCount; texelIndex += 1, channelIndex += 4) {
					rgbaData[channelIndex + 0] = imageAsset->getData()[texelIndex];
					rgbaData[channelIndex + 1] = 0;
					rgbaData[channelIndex + 2] = 0;
					rgbaData[channelIndex + 3] = 255;
				}

				// TODO: Automatically determine channel count.
				const uint8 channelCount = 4;

				qoi_rgba previousTexel = {0, 0, 0, 255};
				qoi_index previousTexelHash = rgbaHash(previousTexel);
				qoi_rgba texelCache[64]{};

				uint8 runLength = 0;

				for (uint32 z = 0; z < imageAsset->getExtent().depth; z += 1) {
					for (uint32 y = 0; y < imageAsset->getExtent().height; y += 1) {
						for (uint32 x = 0; x < imageAsset->getExtent().width; x += 1) {
							const rgba baseTexel = *pointer_cast<const rgba*>(&imageAsset->getData()[x * y * z * channelCount]);

							qoi_rgba texel;
							texel.r = baseTexel.r;
							texel.g = baseTexel.g;
							texel.b = baseTexel.b;
							texel.a = baseTexel.a;

							const qoi_index texelHash = rgbaHash(texel);

							if (texelHash.index == previousTexelHash.index && runLength < 62) {
								runLength += 1;

								continue;
							}

							if (runLength > 0) {
								qoi_run run;
								run.tag = 0b11;
								run.run = runLength;

								outputByteStream.write<qoi_run>(run);

								runLength = 0;
							}
							else if (texel == texelCache[texelHash.index]) {
								qoi_index index;
								index.tag = 0b00;
								index.index = texelHash.index;

								outputByteStream.write<qoi_index>(index);
							}
							else {
								outputByteStream.write<uint32>(0b11111111);
								outputByteStream.write<qoi_rgba>(texel);
							}

							previousTexelHash = texelHash;
							texelCache[texelHash.index] = texel;
						}
					}
				}

				const uint64 outputSize = outputByteStream.getByteOffset();

				BREAKPOINT();

				const std::string outputPath = assetOutputPath + "Image/" + assetName + ".tim";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputSize);

				BREAKPOINT();
			}
	};
}