// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Font/GlyphRasterizer.hpp"
#include "AssetBuilder/Process/Font/FontProcess.hpp"

namespace Trident {
	class BuildFontAssetProcess final : public Process {
		public:
			explicit BuildFontAssetProcess(AssetBuilderLibrary* assetLibrary, GlyphRasterizer* glyphRasterizer) :
				assetLibrary(assetLibrary),
				glyphRasterizer(glyphRasterizer)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[1]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Font/";
				const std::filesystem::recursive_directory_iterator fontDirectoryIterator(rootInputPath / "Asset/Font");

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					const sequence<char8> path = directoryEntry.path().string();
					const sequence<uint8> data = Filesystem::openByteBuffer(path);

					if (!Decoder<FileFormat::TTF>::validateSignature(ByteStream<StreamOperation::READ>(&data[0], data.count(), Endianness::BIG_ENDIAN))) {
						continue;
					}

					std::filesystem::path outputPath = assetOutputPath / directoryEntry.path().filename();
					outputPath.replace_extension("tfo");

					if (std::filesystem::exists(outputPath)) {
						continue;
					}

					const std::string assetNameString = directoryEntry.path().stem().string();
					const view<char8> assetName(&assetNameString[0], assetNameString.size());
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const File<FileFormat::TTF> file = Decoder<FileFormat::TTF>::decode(data);

					SDFGlyphMap sdfGlyphMap;

					glyphRasterizer->generateGlyphAtlas(file, sdfGlyphMap);

					ImageAssetDescriptor imageAssetDescriptor{};
					imageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
					imageAssetDescriptor.format = ImageFormat::R8_UNORM;
					imageAssetDescriptor.extent = {sdfGlyphMap.extent.x, sdfGlyphMap.extent.y, 1};
					imageAssetDescriptor.mipLevelCount = 1;
					imageAssetDescriptor.layerCount = 1;
					imageAssetDescriptor.data = sdfGlyphMap.data;

					const ByteStream<StreamOperation::WRITE> imageStream = ImageProcess::encodeImageAsset(imageAssetDescriptor);

					ImageProcess::createImageAsset(imageAssetDescriptor, imageStream, assetName, rootOutputPath / "Asset/Image/", assetLibrary);

					BuildFont font = {
						.glyphMapImage = assetLibrary->getAsset<ImageAsset>(assetName),
						.ascent = file.ascent,
						.descent = file.descent,
						.lineGap = file.lineGap,
						.unitsPerEm = file.unitsPerEm,
						.glyphs = sdfGlyphMap.glyphs,
						.unicodeMap = file.unicodeMap,
						.kerningPairs = file.kerningPairs,
					};

					const ByteStream<StreamOperation::WRITE> fontStream = FontProcess::encodeFontAsset(identifier, font);

					FontProcess::createFontAsset(identifier, fontStream, assetName, assetOutputPath, assetLibrary);

					ENGINE_INFO("Generated font asset \"[]\" (ID: []).", assetName, identifier);
				}

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
			GlyphRasterizer* glyphRasterizer;
	};
}