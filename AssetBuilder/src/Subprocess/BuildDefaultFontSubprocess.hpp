#pragma once

#include "BuildGlyphMapImageSubprocess.hpp"

namespace ProjectA {
	class BuildDefaultFontSubprocess {
		private:
			struct BitmapGlyph {
				const char8* codePoint;
				uint16 width;
				float32 minCoordinateX;
				float32 minCoordinateY;
			};

		private:
			static constexpr sint16 ASCENT = 9;
			static constexpr sint16 DESCENT = -3;
			static constexpr sint16 LINE_GAP = 0;
			static constexpr BitmapGlyph BITMAP_GLYPHS[] = {
				{"A", 5, 0.0f, 0.0f},
				{"B", 5, 6.0f, 0.0f},
				{"C", 5, 12.0f, 0.0f},
				{"D", 5, 18.0f, 0.0f},
				{"E", 5, 24.0f, 0.0f},
				{"F", 5, 30.0f, 0.0f},
				{"G", 5, 36.0f, 0.0f},
				{"H", 5, 42.0f, 0.0f},
				{"I", 3, 48.0f, 0.0f},
				{"J", 5, 52.0f, 0.0f},
				{"K", 5, 58.0f, 0.0f},
				{"L", 5, 64.0f, 0.0f},
				{"M", 5, 70.0f, 0.0f},
				{"N", 5, 76.0f, 0.0f},
				{"O", 5, 82.0f, 0.0f},
				{"P", 5, 88.0f, 0.0f},
				{"Q", 5, 94.0f, 0.0f},
				{"R", 5, 100.0f, 0.0f},
				{"S", 5, 106.0f, 0.0f},
				{"T", 5, 112.0f, 0.0f},
				{"U", 5, 118.0f, 0.0f},
				{"V", 5, 0.0f, 12.0f},
				{"W", 5, 6.0f, 12.0f},
				{"X", 5, 12.0f, 12.0f},
				{"Y", 5, 18.0f, 12.0f},
				{"Z", 5, 24.0f, 12.0f},
				{"a", 5, 30.0f, 12.0f},
				{"b", 5, 36.0f, 12.0f},
				{"c", 5, 42.0f, 12.0f},
				{"d", 5, 48.0f, 12.0f},
				{"e", 5, 54.0f, 12.0f},
				{"f", 4, 60.0f, 12.0f},
				{"g", 5, 65.0f, 12.0f},
				{"h", 5, 71.0f, 12.0f},
				{"i", 1, 77.0f, 12.0f},
				{"j", 4, 79.0f, 12.0f},
				{"k", 4, 84.0f, 12.0f},
				{"l", 3, 89.0f, 12.0f},
				{"m", 5, 93.0f, 12.0f},
				{"n", 5, 99.0f, 12.0f},
				{"o", 5, 105.0f, 12.0f},
				{"p", 5, 111.0f, 12.0f},
				{"q", 5, 117.0f, 12.0f},
				{"r", 5, 0.0f, 24.0f},
				{"s", 5, 6.0f, 24.0f},
				{"t", 3, 12.0f, 24.0f},
				{"u", 5, 16.0f, 24.0f},
				{"v", 5, 22.0f, 24.0f},
				{"w", 5, 28.0f, 24.0f},
				{"x", 5, 34.0f, 24.0f},
				{"y", 5, 40.0f, 24.0f},
				{"z", 5, 46.0f, 24.0f},
				{"0", 5, 52.0f, 24.0f},
				{"1", 3, 58.0f, 24.0f},
				{"2", 5, 62.0f, 24.0f},
				{"3", 5, 68.0f, 24.0f},
				{"4", 5, 74.0f, 24.0f},
				{"5", 5, 80.0f, 24.0f},
				{"6", 5, 86.0f, 24.0f},
				{"7", 5, 92.0f, 24.0f},
				{"8", 5, 98.0f, 24.0f},
				{"9", 5, 104.0f, 24.0f},
				{".", 1, 110.0f, 24.0f},
				{",", 2, 112.0f, 24.0f},
				{"“", 4, 115.0f, 24.0f},
				{"”", 4, 120.0f, 24.0f},
				{"‘", 2, 0.0f, 36.0f},
				{"’", 2, 3.0f, 36.0f},
				{"\"", 3, 6.0f, 36.0f},
				{"\'", 1, 10.0f, 36.0f},
				{"?", 4, 12.0f, 36.0f},
				{"!", 1, 17.0f, 36.0f},
				{"@", 7, 19.0f, 36.0f},
				{"_", 5, 27.0f, 36.0f},
				{"*", 3, 33.0f, 36.0f},
				{"#", 5, 37.0f, 36.0f},
				{"$", 5, 43.0f, 36.0f},
				{"%", 5, 49.0f, 36.0f},
				{"&", 5, 55.0f, 36.0f},
				{"(", 2, 61.0f, 36.0f},
				{")", 2, 64.0f, 36.0f},
				{"+", 5, 67.0f, 36.0f},
				{"-", 5, 73.0f, 36.0f},
				{"/", 5, 79.0f, 36.0f},
				{":", 1, 85.0f, 36.0f},
				{";", 2, 87.0f, 36.0f},
				{"<", 5, 90.0f, 36.0f},
				{"=", 5, 96.0f, 36.0f},
				{">", 5, 102.0f, 36.0f},
				{"[", 2, 108.0f, 36.0f},
				{"\\", 5, 111.0f, 36.0f},
				{"]", 2, 117.0f, 36.0f},
				{"^", 3, 120.0f, 36.0f},
				{"`", 2, 124.0f, 36.0f},
				{"{", 3, 0.0f, 48.0f},
				{"|", 1, 4.0f, 48.0f},
				{"}", 3, 6.0f, 48.0f},
				{"~", 5, 10.0f, 48.0f},
				{"¡", 1, 16.0f, 48.0f},
				{"¦", 1, 18.0f, 48.0f},
				{"§", 5, 20.0f, 48.0f},
				{"¨", 3, 26.0f, 48.0f},
				{"©", 7, 30.0f, 48.0f},
				{"«", 5, 38.0f, 48.0f},
				{"°", 3, 44.0f, 48.0f},
				{"±", 5, 48.0f, 48.0f},
				{"»", 5, 54.0f, 48.0f},
				{"¿", 4, 60.0f, 48.0f},
				{"Æ", 7, 65.0f, 48.0f},
				{"×", 5, 73.0f, 48.0f},
				{"Ø", 7, 79.0f, 48.0f},
				{"à", 5, 87.0f, 48.0f},
				{"á", 5, 93.0f, 48.0f},
				{"â", 5, 99.0f, 48.0f},
				{"ã", 5, 105.0f, 48.0f},
				{"ä", 5, 111.0f, 48.0f},
				{"ȧ", 5, 117.0f, 48.0f},
				{"æ", 7, 0.0f, 60.0f},
				{"ç", 5, 8.0f, 60.0f},
				{"è", 5, 14.0f, 60.0f},
				{"é", 5, 20.0f, 60.0f},
				{"ê", 5, 26.0f, 60.0f},
				{"ë", 5, 32.0f, 60.0f},
				{"ì", 2, 38.0f, 60.0f},
				{"í", 2, 41.0f, 60.0f},
				{"î", 3, 44.0f, 60.0f},
				{"ï", 3, 48.0f, 60.0f},
				{"ñ", 5, 52.0f, 60.0f},
				{"ò", 5, 58.0f, 60.0f},
				{"ó", 5, 64.0f, 60.0f},
				{"ô", 5, 70.0f, 60.0f},
				{"õ", 5, 76.0f, 60.0f},
				{"ö", 5, 82.0f, 60.0f},
				{"÷", 5, 88.0f, 60.0f},
				{"ø", 5, 94.0f, 60.0f},
				{"ù", 5, 100.0f, 60.0f},
				{"ú", 5, 106.0f, 60.0f},
				{"û", 5, 112.0f, 60.0f},
				{"ü", 5, 118.0f, 60.0f},
				{"Œ", 7, 0.0f, 72.0f},
				{"œ", 7, 8.0f, 72.0f},
				{"€", 5, 16.0f, 72.0f},
				{"™", 6, 22.0f, 72.0f},
				{" ", 4, 29.0f, 72.0f},
			};

		public:
			explicit BuildDefaultFontSubprocess(const std::string& assetOutputPath) {
				const sequence<uint8> buffer = Filesystem::openBuffer("../ProjectA/src/Asset/Image/quiver.png");
				const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(buffer);

				ENGINE_ASSERT(file.format == ImageFormat::R8G8B8A8_UNORM, "The default font must be created from an RGBA PNG.");
				ENGINE_ASSERT(file.extent.depth == 1, "The default font file must not have depth.");

				sequence<uint8> data(file.data.count / 4);

				convertRGBAToRed(file.data, data);

				ImageAssetDescriptor glyphMapImageAssetDescriptor{};
				glyphMapImageAssetDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageAssetDescriptor.extent = file.extent;
				glyphMapImageAssetDescriptor.mipLevelCount = 1;
				glyphMapImageAssetDescriptor.layerCount = 1;
				glyphMapImageAssetDescriptor.data = data;

				FontAssetDescriptor fontAssetDescriptor{};
				fontAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
				fontAssetDescriptor.version = {1, 0, 0};
				fontAssetDescriptor.glyphMapImage = createRef<ImageAsset>(glyphMapImageAssetDescriptor);
				fontAssetDescriptor.lineHeight = ASCENT - DESCENT + LINE_GAP;
				fontAssetDescriptor.glyphs = sequence<Glyph>(144);

				for (uint32 glyphIndex = 0; glyphIndex < fontAssetDescriptor.glyphs.count; glyphIndex += 1) {
					const BitmapGlyph& bitmapGlyph = BITMAP_GLYPHS[glyphIndex];

					uint32 byteOffset = 0;

					const uint16 codePoint = GUILayer::decodeUtf16CodePoint(bitmapGlyph.codePoint, byteOffset);

					fontAssetDescriptor.glyphs[glyphIndex].minCoordinate = {bitmapGlyph.minCoordinateX / file.extent.width, bitmapGlyph.minCoordinateY / file.extent.height};
					fontAssetDescriptor.glyphs[glyphIndex].maxCoordinate = {(bitmapGlyph.minCoordinateX + bitmapGlyph.width) / file.extent.width, (bitmapGlyph.minCoordinateY + fontAssetDescriptor.lineHeight) / file.extent.height};
					fontAssetDescriptor.glyphs[glyphIndex].width = bitmapGlyph.width;
					fontAssetDescriptor.glyphs[glyphIndex].advanceWidth = bitmapGlyph.width + 1;
					fontAssetDescriptor.glyphs[glyphIndex].leftSideBearing = 0;
					fontAssetDescriptor.glyphs[glyphIndex].whitespace = codePoint == ' ';

					fontAssetDescriptor.unicodeMap.insert({codePoint, glyphIndex});
				}

				const uint32 glyphCount = static_cast<uint32>(fontAssetDescriptor.glyphs.count);
				const uint64 outputSize =
					7 * sizeof(uint32) + (4 * sizeof(float32) + 3 * sizeof(uint16)) * glyphCount +
					(sizeof(uint16) + sizeof(uint32)) * fontAssetDescriptor.unicodeMap.size();

				uint32 glyphMapImageAssetIdentifier;

				const BuildGlyphMapImageSubprocess buildGlyphMapImageSubprocess(assetOutputPath, "DefaultFont", fontAssetDescriptor.glyphMapImage, glyphMapImageAssetIdentifier);

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::TFO);
				outputByteStream.write<uint32>(fontAssetDescriptor.identifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(glyphMapImageAssetIdentifier);
				outputByteStream.write<sint16>(ASCENT);
				outputByteStream.write<sint16>(DESCENT);
				outputByteStream.write<sint16>(LINE_GAP);
				outputByteStream.jump(sizeof(uint16));
				outputByteStream.write<uint32>(glyphCount);

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<float32>(glyph.minCoordinate.x);
					outputByteStream.write<float32>(glyph.minCoordinate.y);
					outputByteStream.write<float32>(glyph.maxCoordinate.x);
					outputByteStream.write<float32>(glyph.maxCoordinate.y);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.width);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.advanceWidth);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<sint16>(glyph.leftSideBearing);
				}

				for (const auto& unicodeEntry : fontAssetDescriptor.unicodeMap) {
					outputByteStream.write<uint16>(unicodeEntry.first);
				}

				for (const auto& unicodeEntry : fontAssetDescriptor.unicodeMap) {
					outputByteStream.write<uint32>(unicodeEntry.second);
				}

				const std::string outputPath = assetOutputPath + "/Font/DefaultFont.tfo";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				ENGINE_INFO("Generated default font asset (ID: []).", fontAssetDescriptor.identifier);
			}
	};
}