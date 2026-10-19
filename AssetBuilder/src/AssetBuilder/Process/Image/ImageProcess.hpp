// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/File/QOI.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class ImageProcess : public Process {
		public:
			static ByteStream<StreamOperation::WRITE> encodeImageAsset(const ImageAssetDescriptor& image) {
				const uint64 maxStreamSize = calculateMaxStreamSize(image);

				ByteStream<StreamOperation::WRITE> stream(maxStreamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::IMAGE);
				stream.write<uint32>(image.identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<ImageFormat>(image.format);
				stream.write<Extent3D>(image.extent);
				stream.write<uint32>(image.mipLevelCount);
				stream.write<uint32>(image.layerCount);

				switch (image.format) {
					case ImageFormat::R8_UNORM: {
						std::copy(image.data.begin(), image.data.end(), stream.getBytePosition());

						stream.jump(image.data.count());

						break;
					}
					case ImageFormat::R8G8B8_UNORM:
					case ImageFormat::R8G8B8A8_UNORM: {
						const uint32 channelCount = getChannelCount(image.format);

						uint32 texelIndex = 0;
						qoi_rgba texelCache[64]{};
						qoi_rgba previousTexel = {0, 0, 0, 255};

						// TODO(asset-builder/image): Refactor.
						qoi_index previousTexelHash = rgbaHash(previousTexel);

						uint8 runLength = 0;
						uint32 layerIndex = 0;

						while (layerIndex < image.layerCount) {
							uint32 z = 0;

							while (z < image.extent.z) {
								uint32 y = 0;

								while (y < image.extent.y) {
									uint32 x = 0;

									while (x < image.extent.x) {
										qoi_rgba texel = *pointer_cast<const qoi_rgba*>(&image.data[texelIndex * channelCount]);

										if (image.format == ImageFormat::R8G8B8_UNORM) {
											texel.a = previousTexel.a;
										}

										if (texel == previousTexel && runLength < 62) {
											runLength += 1;
											x += 1;
											texelIndex += 1;

											continue;
										}

										if (runLength > 0) {
											qoi_run run{};
											run.tag = 0b11;
											run.run = runLength - 1;

											stream.write<qoi_run>(run);

											runLength = 0;

											if (texel == previousTexel) {
												runLength = 1;
												x += 1;
												texelIndex += 1;

												continue;
											}
										}

										const qoi_index texelHash = rgbaHash(texel);

										if (texel == texelCache[texelHash.index]) {
											qoi_index index{};
											index.tag = 0b00;
											index.index = texelHash.index;

											stream.write<qoi_index>(index);

											previousTexel = texel;
											previousTexelHash = texelHash;
											x += 1;
											texelIndex += 1;

											continue;
										}
										else {
											texelCache[texelHash.index] = texel;

											if (texel.a == previousTexel.a) {
												const sint8 dr = texel.r - previousTexel.r;
												const sint8 dg = texel.g - previousTexel.g;
												const sint8 db = texel.b - previousTexel.b;
												const sint8 dr_dg = dr - dg;
												const sint8 db_dg = db - dg;

												if (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 && db >= -2 && db <= 1) {
													qoi_diff diff{};
													diff.tag = 0b01;
													diff.dr = dr + 2;
													diff.dg = dg + 2;
													diff.db = db + 2;

													stream.write<qoi_diff>(diff);
												}
												else if (dr_dg >= -8 && dr_dg <= 7 && dg >= -32 && dg <= 31 && db_dg >= -8 && db_dg <= 7) {
													qoi_luma luma{};
													luma.tag = 0b10;
													luma.dg = dg + 32;
													luma.dr_dg = dr_dg + 8;
													luma.db_dg = db_dg + 8;

													stream.write<qoi_luma>(luma);
												}
												else {
													qoi_rgb rgb{};
													rgb.r = texel.r;
													rgb.g = texel.g;
													rgb.b = texel.b;

													stream.write<uint8>(0b11111110);
													stream.write<qoi_rgb>(rgb);
												}
											}
											else {
												stream.write<uint8>(0b11111111);
												stream.write<qoi_rgba>(texel);
											}
										}

										previousTexel = texel;
										previousTexelHash = texelHash;
										x += 1;
										texelIndex += 1;
									}

									y += 1;
								}

								z += 1;
							}

							layerIndex += 1;
						}

						if (runLength > 0) {
							qoi_run run{};
							run.tag = 0b11;
							run.run = runLength - 1;

							stream.write<qoi_run>(run);
						}

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				return stream;
			}

			static void createImageAsset(const ImageAssetDescriptor& image, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, std::filesystem::path outputDirectoryPath, AssetBuilderLibrary* assetLibrary) {
				const std::string assetOutputPathString = outputDirectoryPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tia", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.getByteOffset());

				assetLibrary->registerAsset(image.identifier, assetName);
			}

		private:
			static uint64 calculateMaxStreamSize(const ImageAssetDescriptor& image) {
				// TODO(asset-builder/image): Include mip levels.
				uint64 maxDataSize = image.extent.x * image.extent.y * image.extent.z * image.layerCount;

				switch (image.format) {
					case ImageFormat::R8_UNORM: {
						maxDataSize *= sizeof(uint8);

						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						// Worst case: every chunk is QOI_RGB.
						maxDataSize *= sizeof(uint8) + sizeof(qoi_rgb);

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						// Worst case: every chunk is QOI_RGBA.
						maxDataSize *= sizeof(uint8) + sizeof(qoi_rgba);

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				return (
					sizeof(AssetHeader) +
					sizeof(ImageFormat) +
					sizeof(Extent3D) +
					sizeof(uint32) * 2 +
					maxDataSize
				);
			}

		public:
			explicit ImageProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[1]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path outputDirectoryPath = rootOutputPath / "Asset/Image/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path outputPath = outputDirectoryPath / std::string_view(assetName);
				outputPath.replace_extension("tia");

				if (std::filesystem::exists(outputPath)) {
					return 0;
				}

				const ImageAssetDescriptor image = createImage(rootInputPath);
				const ByteStream<StreamOperation::WRITE> stream = encodeImageAsset(image);

				createImageAsset(image, stream, assetName, outputDirectoryPath, assetLibrary);

				ENGINE_INFO("Generated image asset \"[]\" (ID: []).", assetName, image.identifier);

				return 0;
			}

		protected:
			virtual view<char8> getAssetName() const = 0;

			virtual ImageAssetDescriptor createImage(std::filesystem::path rootInputPath) const = 0;

		protected:
			AssetBuilderLibrary* assetLibrary;
	};
}