// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class BuildGlyphMapImageSubprocess {
		private:
			static constexpr uint64 IMAGE_ASSET_HEADER_SIZE = 9 * sizeof(uint32);

		public:
			explicit BuildGlyphMapImageSubprocess(const std::filesystem::path& assetOutputPath, view<char8> assetName, ref<ImageAsset> imageAsset, uint32& assetIdentifier) {
				uint64 maxOutputDataSize;

				switch (imageAsset->getFormat()) {
					case ImageFormat::R8_UNORM: {
						maxOutputDataSize = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * sizeof(uint8);

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						// In the worst case, all chunks are QOI_RGBA, which is 5 bytes.
						maxOutputDataSize = imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * (sizeof(uint8) + sizeof(qoi_rgba));

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				assetIdentifier = AssetLibrary::createAssetIdentifier();

				// TODO: Add support for header structures which are much more readable.
				ByteStream<StreamOperation::WRITE> outputByteStream(IMAGE_ASSET_HEADER_SIZE + maxOutputDataSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::TIM);
				outputByteStream.write<uint32>(assetIdentifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(static_cast<uint32>(imageAsset->getFormat()));
				outputByteStream.write<Extent3D>(imageAsset->getExtent());
				outputByteStream.write<uint32>(imageAsset->getMipLevelCount());
				outputByteStream.write<uint32>(imageAsset->getLayerCount());

				switch (imageAsset->getFormat()) {
					case ImageFormat::R8_UNORM: {
						const view<uint8> data = imageAsset->getData();

						std::copy(data.begin(), data.end(), outputByteStream.getBytePosition());

						outputByteStream.jump(data.count());

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						sequence<uint8> perChannelData(imageAsset->getData().count() * 4);

						convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &perChannelData[0], perChannelData.count());

						const qoi_rgba* data = pointer_cast<const qoi_rgba*>(&perChannelData[0]);

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

										const qoi_rgba texel = data[texelIndex];
										const qoi_index texelHash = rgbaHash(texel);

										if (texelHash.index == previousTexelHash.index && runLength != 62) {
											runLength += 1;

											continue;
										}

										if (runLength > 0) {
											qoi_run run{};
											run.tag = 0b11;
											run.run = runLength - 1;

											outputByteStream.write<qoi_run>(run);

											runLength = 0;
										}

										if (texel == texelCache[texelHash.index]) {
											qoi_index index{};
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
							qoi_run run{};
							run.tag = 0b11;
							run.run = runLength - 1;

							outputByteStream.write<qoi_run>(run);
						}

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				const uint64 outputSize = outputByteStream.getByteOffset();
				const std::string assetOutputPathString = assetOutputPath.string();

				// Count '\0'.
				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 11);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy_n("Image/", 6, &outputPath[assetOutputPathString.size()]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size() + 6]);
				std::copy_n(".tim", 4, &outputPath[assetOutputPathString.size() + assetName.count() + 6]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputSize);

				ENGINE_INFO("Generated glyph map image asset \"[]\" (ID: []).", outputPath, assetIdentifier);
			}
	};
}