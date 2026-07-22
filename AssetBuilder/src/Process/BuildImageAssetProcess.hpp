// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/BuildAssetMap.hpp"
#include "../File/PNG.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/File/QOI.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	inline void ByteStream<StreamOperation::WRITE>::write(const AssetHeader& header, bool swapEndianness) {
		write<AssetSignature>(header.signature, swapEndianness);
		write<uint32>(header.identifier, swapEndianness);
		write<VersionNumber>(header.version, swapEndianness);
	}

	template<>
	inline void ByteStream<StreamOperation::WRITE>::write(const ImageAssetHeader& header, bool swapEndianness) {
		write<AssetHeader>(header, swapEndianness);
		write<uint32>(header.format, swapEndianness);
		write<uint32>(header.extent.width, swapEndianness);
		write<uint32>(header.extent.height, swapEndianness);
		write<uint32>(header.extent.depth, swapEndianness);
		write<uint32>(header.mipLevelCount, swapEndianness);
		write<uint32>(header.layerCount, swapEndianness);
	}

	class BuildImageAssetProcess final : public Process {
		public:
			static void encodeImageAsset(const std::filesystem::path& assetOutputPath, view<char8> assetName, ImageAssetDescriptor& descriptor) {
				if (descriptor.identifier == max<uint32>()) {
					descriptor.identifier = AssetLibrary::createAssetIdentifier();
				}

				ImageAssetHeader imageAssetHeader{};
				imageAssetHeader.signature = AssetSignature::IMAGE;
				imageAssetHeader.identifier = descriptor.identifier;
				imageAssetHeader.version = descriptor.version;
				imageAssetHeader.format = static_cast<uint32>(descriptor.format);
				imageAssetHeader.extent = descriptor.extent;
				imageAssetHeader.mipLevelCount = descriptor.mipLevelCount;
				imageAssetHeader.layerCount = descriptor.layerCount;

				// TODO(asset-builder/image): Include mip levels.
				uint64 maxOutputDataSize = descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth * descriptor.layerCount;

				switch (descriptor.format) {
					case ImageFormat::R8_UNORM: {
						maxOutputDataSize *= sizeof(uint8);

						break;
					}
					case ImageFormat::R8G8B8_UNORM: {
						// In the worst case, all chunks are QOI_RGB, which is 4 bytes.
						maxOutputDataSize *= sizeof(uint8) + sizeof(qoi_rgb);

						break;
					}
					case ImageFormat::R8G8B8A8_UNORM: {
						// In the worst case, all chunks are QOI_RGBA, which is 5 bytes.
						maxOutputDataSize *= sizeof(uint8) + sizeof(qoi_rgba);

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				ByteStream<StreamOperation::WRITE> outputByteStream(sizeof(imageAssetHeader) + maxOutputDataSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write(imageAssetHeader);

				switch (descriptor.format) {
					case ImageFormat::R8_UNORM: {
						std::copy(descriptor.data.begin(), descriptor.data.end(), outputByteStream.getBytePosition());

						outputByteStream.jump(descriptor.data.count());

						break;
					}
					case ImageFormat::R8G8B8_UNORM:
					case ImageFormat::R8G8B8A8_UNORM: {
						const uint32 channelCount = getChannelCount(descriptor.format);

						uint32 x;
						uint32 y;
						uint32 z = 0;
						uint32 texelIndex = 0;
						qoi_rgba texelCache[64]{};
						qoi_rgba previousTexel = {0, 0, 0, 255};
						qoi_index previousTexelHash = rgbaHash(previousTexel);
						uint8 runLength = 0;

						while (z < descriptor.extent.depth) {
							y = 0;

							while (y < descriptor.extent.height) {
								x = 0;

								while (x < descriptor.extent.width) {
									qoi_rgba texel = *pointer_cast<const qoi_rgba*>(&descriptor.data[texelIndex * channelCount]);

									if (descriptor.format == ImageFormat::R8G8B8_UNORM) {
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

										outputByteStream.write<qoi_run>(run);

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

										outputByteStream.write<qoi_index>(index);

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

												outputByteStream.write<qoi_diff>(diff);
											}
											else if (dr_dg >= -8 && dr_dg <= 7 && dg >= -32 && dg <= 31 && db_dg >= -8 && db_dg <= 7) {
												qoi_luma luma{};
												luma.tag = 0b10;
												luma.dg = dg + 32;
												luma.dr_dg = dr_dg + 8;
												luma.db_dg = db_dg + 8;

												outputByteStream.write<qoi_luma>(luma);
											}
											else {
												qoi_rgb rgb{};
												rgb.r = texel.r;
												rgb.g = texel.g;
												rgb.b = texel.b;

												outputByteStream.write<uint8>(0b11111110);
												outputByteStream.write<qoi_rgb>(rgb);
											}
										}
										else {
											outputByteStream.write<uint8>(0b11111111);
											outputByteStream.write<qoi_rgba>(texel);
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
			void encodeImageAsset(const std::filesystem::directory_entry& directoryEntry, const std::filesystem::path& assetOutputPath) {
				std::filesystem::path finalAssetOutputPath = assetOutputPath / "Image" / directoryEntry.path().filename();
				finalAssetOutputPath.replace_extension("tia");

				if (std::filesystem::exists(finalAssetOutputPath)) {
					ENGINE_INFO("Image asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

					return;
				}

				const sequence<char8> path = directoryEntry.path().string();
				const std::string assetNameString = directoryEntry.path().stem().string();
				const view<char8> assetName(&assetNameString[0], assetNameString.size());
				const sequence<uint8> inputData = Filesystem::openByteBuffer(path);

				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
				imageAssetDescriptor.mipLevelCount = 1;
				imageAssetDescriptor.layerCount = 1;

				if (Decoder<FileFormat::PNG>::validateSignature(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::BIG_ENDIAN))) {
					const File<FileFormat::PNG> file = Decoder<FileFormat::PNG>::decode(inputData);

					imageAssetDescriptor.format = file.format;
					imageAssetDescriptor.extent = file.extent;
					imageAssetDescriptor.data = file.data;
				}
				else if (Decoder<FileFormat::QOI>::validateSignature(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::LITTLE_ENDIAN))) {
					const File<FileFormat::QOI> file = Decoder<FileFormat::QOI>::decode(inputData);

					imageAssetDescriptor.format = file.format;
					imageAssetDescriptor.extent = file.extent;
					imageAssetDescriptor.data = file.data;
				}
				else {
					ENGINE_ERROR_UNSUPPORTED();
				}

				encodeImageAsset(assetOutputPath, assetName, imageAssetDescriptor);

				assetMap->insert(imageAssetDescriptor.identifier, assetName);
			}

		public:
			explicit BuildImageAssetProcess(BuildAssetMap* assetMap) :
				assetMap(assetMap)
			{}

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

		private:
			BuildAssetMap* assetMap;
	};
}