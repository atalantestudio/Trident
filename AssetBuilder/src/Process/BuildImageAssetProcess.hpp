// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class BuildImageAssetProcess : public Process {
		public:
			static void encodeImageAsset(const std::filesystem::path& assetOutputPath, view<char8> assetName, ImageAssetDescriptor& descriptor) {
				descriptor.identifier = AssetLibrary::createAssetIdentifier();

				ImageAssetHeader imageAssetHeader{};
				imageAssetHeader.signature = AssetSignature::IMAGE_ASSET;
				imageAssetHeader.identifier = descriptor.identifier;
				imageAssetHeader.version = descriptor.version;
				imageAssetHeader.format = static_cast<uint32>(descriptor.format);
				imageAssetHeader.extent = descriptor.extent;
				imageAssetHeader.mipLevelCount = descriptor.mipLevelCount;
				imageAssetHeader.layerCount = descriptor.layerCount;

				uint64 maxOutputDataSize;

				switch (descriptor.format) {
					case ImageFormat::R8_UNORM: {
						maxOutputDataSize = descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth * sizeof(uint8);

						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						// In the worst case, all chunks are QOI_RGB, which is 4 bytes.
						maxOutputDataSize = descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth * (sizeof(uint8) + sizeof(qoi_rgb));

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						// In the worst case, all chunks are QOI_RGBA, which is 5 bytes.
						maxOutputDataSize = descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth * (sizeof(uint8) + sizeof(qoi_rgba));

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				ByteStream<StreamOperation::WRITE> outputByteStream(sizeof(imageAssetHeader) + maxOutputDataSize, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&imageAssetHeader), sizeof(imageAssetHeader), outputByteStream.getBytePosition());

				outputByteStream.jump(sizeof(imageAssetHeader));

				switch (descriptor.format) {
					case ImageFormat::R8_UNORM: {
						std::copy(descriptor.data.begin(), descriptor.data.end(), outputByteStream.getBytePosition());

						outputByteStream.jump(descriptor.data.count());

						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						const qoi_rgb* data = pointer_cast<const qoi_rgb*>(&descriptor.data[0]);

						qoi_rgb texelCache[64]{};
						qoi_rgb previousTexel = {0, 0, 0};
						qoi_index previousTexelHash = rgbHash(previousTexel);
						uint8 runLength = 0;
						uint32 texelIndex = 0;

						for (uint32 z = 0; z < descriptor.extent.depth; z += 1) {
							for (uint32 y = 0; y < descriptor.extent.height; y += 1) {
								for (uint32 x = 0; x < descriptor.extent.width; x += 1, texelIndex += 1) {
									texelCache[previousTexelHash.index] = previousTexel;

									const qoi_rgb texel = data[texelIndex];
									const qoi_index texelHash = rgbHash(texel);

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
										outputByteStream.write<uint8>(0b11111110);
										outputByteStream.write<qoi_rgb>(texel);
									}

									previousTexel = texel;
									previousTexelHash = texelHash;
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
					case ImageFormat::R8G8B8A8_UNORM: {
						const qoi_rgba* data = pointer_cast<const qoi_rgba*>(&descriptor.data[0]);

						qoi_rgba texelCache[64]{};
						qoi_rgba previousTexel = {0, 0, 0, 255};
						qoi_index previousTexelHash = rgbaHash(previousTexel);
						uint8 runLength = 0;
						uint32 texelIndex = 0;

						for (uint32 z = 0; z < descriptor.extent.depth; z += 1) {
							for (uint32 y = 0; y < descriptor.extent.height; y += 1) {
								for (uint32 x = 0; x < descriptor.extent.width; x += 1, texelIndex += 1) {
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
				std::copy_n(".tia", 4, &outputPath[assetOutputPathString.size() + assetName.count() + 6]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputSize);

				ENGINE_INFO("Generated image asset \"[]\" (ID: []).", outputPath, imageAssetHeader.identifier);
			}

		private:
			static void encodeImageAsset(const std::filesystem::directory_entry& directoryEntry, const std::filesystem::path& assetOutputPath) {
				const sequence<char8> path = directoryEntry.path().string();
				const std::string assetNameString = directoryEntry.path().stem().string();
				const view<char8> assetName(&assetNameString[0], assetNameString.size());
				const sequence<uint8> inputData = Filesystem::openByteBuffer(path);

				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.mipLevelCount = 1;
				imageAssetDescriptor.layerCount = 1;

				if (Filesystem::validateSignature<FileFormat::PNG>(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::BIG_ENDIAN))) {
					const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(inputData);

					imageAssetDescriptor.format = file.format;
					imageAssetDescriptor.extent = file.extent;
					imageAssetDescriptor.data = file.data;
				}
				else if (Filesystem::validateSignature<FileFormat::QOI>(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::LITTLE_ENDIAN))) {
					const File<FileFormat::QOI> file = Filesystem::decode<FileFormat::QOI>(inputData);

					imageAssetDescriptor.format = file.format;
					imageAssetDescriptor.extent = file.extent;
					imageAssetDescriptor.data = file.data;
				}
				else {
					ENGINE_ERROR_UNSUPPORTED();
				}

				encodeImageAsset(assetOutputPath, assetName, imageAssetDescriptor);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[1]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::filesystem::recursive_directory_iterator imageDirectoryIterator(rootInputPath / "Asset/Image");

				for (const std::filesystem::directory_entry& directoryEntry : imageDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					encodeImageAsset(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building image assets.");

				return 0;
			}
	};
}