#pragma once

#include "BuildGlyphMapImageSubprocess.hpp"

namespace Trident {
	class BuildDefaultFontSubprocess {
		private:
			struct BitmapGlyph {
				const char8* codePoint;
				svec2<float32> minCoordinate;
				uint16 width;
				uint16 height;
				sint16 topSideBearing;
			};

		private:
			static constexpr char8* DEFAULT_FONT_NAME = "DefaultFont";

			static constexpr sint16 ASCENT = 9;
			static constexpr sint16 DESCENT = -3;
			static constexpr sint16 LINE_GAP = 0;
			static constexpr BitmapGlyph BITMAP_GLYPHS[] = {
				{"A", {0.0f, 0.0f}, 5, 7, 2},
				{"B", {6.0f, 0.0f}, 5, 7, 2},
				{"C", {12.0f, 0.0f}, 5, 7, 2},
				{"D", {18.0f, 0.0f}, 5, 7, 2},
				{"E", {24.0f, 0.0f}, 5, 7, 2},
				{"F", {30.0f, 0.0f}, 5, 7, 2},
				{"G", {36.0f, 0.0f}, 5, 7, 2},
				{"H", {42.0f, 0.0f}, 5, 7, 2},
				{"I", {48.0f, 0.0f}, 3, 7, 2},
				{"J", {52.0f, 0.0f}, 5, 8, 2},
				{"K", {58.0f, 0.0f}, 5, 7, 2},
				{"L", {64.0f, 0.0f}, 5, 7, 2},
				{"M", {70.0f, 0.0f}, 5, 7, 2},
				{"N", {76.0f, 0.0f}, 5, 7, 2},
				{"O", {82.0f, 0.0f}, 5, 7, 2},
				{"P", {88.0f, 0.0f}, 5, 7, 2},
				{"Q", {94.0f, 0.0f}, 5, 8, 2},
				{"R", {100.0f, 0.0f}, 5, 7, 2},
				{"S", {106.0f, 0.0f}, 5, 7, 2},
				{"T", {112.0f, 0.0f}, 5, 7, 2},
				{"U", {118.0f, 0.0f}, 5, 7, 2},
				{"V", {0.0f, 12.0f}, 5, 7, 2},
				{"W", {6.0f, 12.0f}, 5, 7, 2},
				{"X", {12.0f, 12.0f}, 5, 7, 2},
				{"Y", {18.0f, 12.0f}, 5, 7, 2},
				{"Z", {24.0f, 12.0f}, 5, 7, 2},
				{"a", {30.0f, 12.0f}, 5, 5, 4},
				{"b", {36.0f, 12.0f}, 5, 7, 2},
				{"c", {42.0f, 12.0f}, 5, 7, 4},
				{"d", {48.0f, 12.0f}, 5, 7, 2},
				{"e", {54.0f, 12.0f}, 5, 7, 4},
				{"f", {60.0f, 12.0f}, 4, 12, 0},
				{"g", {65.0f, 12.0f}, 5, 8, 4},
				{"h", {71.0f, 12.0f}, 5, 7, 2},
				{"i", {77.0f, 12.0f}, 1, 7, 2},
				{"j", {79.0f, 12.0f}, 4, 9, 2},
				{"k", {84.0f, 12.0f}, 4, 7, 2},
				{"l", {89.0f, 12.0f}, 3, 7, 2},
				{"m", {93.0f, 12.0f}, 5, 5, 4},
				{"n", {99.0f, 12.0f}, 5, 5, 4},
				{"o", {105.0f, 12.0f}, 5, 5, 4},
				{"p", {111.0f, 12.0f}, 5, 7, 4},
				{"q", {117.0f, 12.0f}, 5, 7, 4},
				{"r", {0.0f, 24.0f}, 5, 5, 4},
				{"s", {6.0f, 24.0f}, 5, 5, 4},
				{"t", {12.0f, 24.0f}, 3, 7, 2},
				{"u", {16.0f, 24.0f}, 5, 5, 4},
				{"v", {22.0f, 24.0f}, 5, 5, 4},
				{"w", {28.0f, 24.0f}, 5, 5, 4},
				{"x", {34.0f, 24.0f}, 5, 5, 4},
				{"y", {40.0f, 24.0f}, 5, 8, 4},
				{"z", {46.0f, 24.0f}, 5, 8, 4},
				{"0", {52.0f, 24.0f}, 5, 7, 2},
				{"1", {58.0f, 24.0f}, 3, 7, 2},
				{"2", {62.0f, 24.0f}, 5, 7, 2},
				{"3", {68.0f, 24.0f}, 5, 7, 2},
				{"4", {74.0f, 24.0f}, 5, 7, 2},
				{"5", {80.0f, 24.0f}, 5, 7, 2},
				{"6", {86.0f, 24.0f}, 5, 7, 2},
				{"7", {92.0f, 24.0f}, 5, 7, 2},
				{"8", {98.0f, 24.0f}, 5, 7, 2},
				{"9", {104.0f, 24.0f}, 5, 7, 2},
				{".", {110.0f, 24.0f}, 1, 1, 8},
				{",", {112.0f, 24.0f}, 2, 3, 8},
				{"“", {115.0f, 24.0f}, 4, 3, 2},
				{"”", {120.0f, 24.0f}, 4, 3, 6},
				{"‘", {0.0f, 36.0f}, 2, 3, 2},
				{"’", {3.0f, 36.0f}, 2, 3, 2},
				{"\"", {6.0f, 36.0f}, 3, 2, 2},
				{"'", {10.0f, 36.0f}, 1, 2, 2},
				{"?", {12.0f, 36.0f}, 4, 7, 2},
				{"!", {17.0f, 36.0f}, 1, 7, 2},
				{"@", {19.0f, 36.0f}, 7, 7, 2},
				{"_", {27.0f, 36.0f}, 5, 1, 8},
				{"*", {33.0f, 36.0f}, 3, 3, 2},
				{"#", {37.0f, 36.0f}, 5, 7, 2},
				{"$", {43.0f, 36.0f}, 5, 7, 2},
				{"%", {49.0f, 36.0f}, 5, 7, 2},
				{"&", {55.0f, 36.0f}, 5, 7, 2},
				{"(", {61.0f, 36.0f}, 2, 9, 1},
				{")", {64.0f, 36.0f}, 2, 9, 1},
				{"+", {67.0f, 36.0f}, 5, 5, 4},
				{"-", {73.0f, 36.0f}, 5, 1, 6},
				{"/", {79.0f, 36.0f}, 5, 7, 2},
				{":", {85.0f, 36.0f}, 1, 5, 4},
				{";", {87.0f, 36.0f}, 2, 7, 4},
				{"<", {90.0f, 36.0f}, 5, 5, 4},
				{"=", {96.0f, 36.0f}, 5, 3, 5},
				{">", {102.0f, 36.0f}, 5, 5, 4},
				{"[", {108.0f, 36.0f}, 2, 9, 1},
				{"\\", {111.0f, 36.0f}, 5, 7, 2},
				{"]", {117.0f, 36.0f}, 2, 9, 1},
				{"^", {120.0f, 36.0f}, 3, 2, 1},
				{"`", {124.0f, 36.0f}, 2, 2, 1},
				{"{", {0.0f, 48.0f}, 3, 9, 1},
				{"|", {4.0f, 48.0f}, 1, 9, 1},
				{"}", {6.0f, 48.0f}, 3, 9, 1},
				{"~", {10.0f, 48.0f}, 5, 3, 5},
				{"¡", {16.0f, 48.0f}, 1, 7, 2},
				{"¦", {18.0f, 48.0f}, 1, 9, 1},
				{"§", {20.0f, 48.0f}, 5, 7, 2},
				{"¨", {26.0f, 48.0f}, 3, 1, 2},
				{"©", {30.0f, 48.0f}, 7, 7, 2},
				{"«", {38.0f, 48.0f}, 5, 3, 5},
				{"°", {44.0f, 48.0f}, 3, 3, 3},
				{"±", {48.0f, 48.0f}, 5, 5, 4},
				{"»", {54.0f, 48.0f}, 5, 3, 5},
				{"¿", {60.0f, 48.0f}, 4, 7, 2},
				{"Æ", {65.0f, 48.0f}, 7, 7, 2},
				{"×", {73.0f, 48.0f}, 5, 5, 4},
				{"Ø", {79.0f, 48.0f}, 7, 7, 2},
				{"à", {87.0f, 48.0f}, 5, 8, 1},
				{"á", {93.0f, 48.0f}, 5, 8, 1},
				{"â", {99.0f, 48.0f}, 5, 8, 1},
				{"ã", {105.0f, 48.0f}, 5, 8, 1},
				{"ä", {111.0f, 48.0f}, 5, 7, 2},
				{"\xC8\xA7", {117.0f, 48.0f}, 5, 7, 2}, // 'ȧ'
				{"æ", {0.0f, 60.0f}, 7, 5, 4},
				{"ç", {8.0f, 60.0f}, 5, 7, 4},
				{"è", {14.0f, 60.0f}, 5, 8, 1},
				{"é", {20.0f, 60.0f}, 5, 8, 1},
				{"ê", {26.0f, 60.0f}, 5, 8, 1},
				{"ë", {32.0f, 60.0f}, 5, 7, 2},
				{"ì", {38.0f, 60.0f}, 2, 8, 1},
				{"í", {41.0f, 60.0f}, 2, 8, 1},
				{"î", {44.0f, 60.0f}, 3, 8, 1},
				{"ï", {48.0f, 60.0f}, 3, 7, 2},
				{"ñ", {52.0f, 60.0f}, 5, 8, 1},
				{"ò", {58.0f, 60.0f}, 5, 8, 1},
				{"ó", {64.0f, 60.0f}, 5, 8, 1},
				{"ô", {70.0f, 60.0f}, 5, 8, 1},
				{"õ", {76.0f, 60.0f}, 5, 8, 1},
				{"ö", {82.0f, 60.0f}, 5, 7, 2},
				{"÷", {88.0f, 60.0f}, 5, 5, 4},
				{"ø", {94.0f, 60.0f}, 5, 5, 4},
				{"ù", {100.0f, 60.0f}, 5, 8, 1},
				{"ú", {106.0f, 60.0f}, 5, 8, 1},
				{"û", {112.0f, 60.0f}, 5, 8, 1},
				{"ü", {118.0f, 60.0f}, 5, 7, 2},
				{"Œ", {0.0f, 72.0f}, 7, 7, 2},
				{"œ", {8.0f, 72.0f}, 7, 5, 4},
				{"€", {16.0f, 72.0f}, 5, 7, 2},
				{"™", {22.0f, 72.0f}, 6, 3, 3},
				{" ", {29.0f, 72.0f}, 4, 0, 0},
			};

		public:
			explicit BuildDefaultFontSubprocess(const std::filesystem::path& assetOutputPath) {
				const sequence<uint8> buffer = Filesystem::openByteBuffer("../Trident/src/Asset/Image/quiver.png");
				const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(buffer);

				ENGINE_ASSERT(file.format == ImageFormat::R8G8B8A8_UNORM, "The default font must be created from an RGBA PNG.");
				ENGINE_ASSERT(file.extent.depth == 1, "The default font file must not have depth.");

				sequence<uint8> data(file.data.count() / 4);

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

				UTF8Stream stream;

				for (uint32 glyphIndex = 0; glyphIndex < fontAssetDescriptor.glyphs.count(); glyphIndex += 1) {
					const BitmapGlyph& bitmapGlyph = BITMAP_GLYPHS[glyphIndex];

					stream = UTF8Stream(view<char8>(bitmapGlyph.codePoint));

					const uint32 codePoint = stream.peek();

					Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];
					glyph.minCoordinate.x = bitmapGlyph.minCoordinate.x / file.extent.width;
					glyph.minCoordinate.y = (bitmapGlyph.minCoordinate.y + bitmapGlyph.topSideBearing) / file.extent.height;
					glyph.maxCoordinate.x = (bitmapGlyph.minCoordinate.x + bitmapGlyph.width) / file.extent.width;
					glyph.maxCoordinate.y = (bitmapGlyph.minCoordinate.y + bitmapGlyph.topSideBearing + bitmapGlyph.height) / file.extent.height;
					glyph.width = bitmapGlyph.width;
					glyph.advanceWidth = bitmapGlyph.width + 1;
					glyph.height = bitmapGlyph.height;
					glyph.leftSideBearing = 0;
					glyph.topSideBearing = bitmapGlyph.topSideBearing;
					glyph.whitespace = codePoint == ' ';

					fontAssetDescriptor.unicodeMap.insert({static_cast<uint16>(codePoint), glyphIndex});
				}

				const uint32 glyphCount = static_cast<uint32>(fontAssetDescriptor.glyphs.count());
				const uint64 outputSize =
					sizeof(uint32) * 7 +
					(sizeof(svec2<float32>) * 2 + sizeof(uint16) * 3 + sizeof(sint16) * 2) * glyphCount +
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

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<float32>(glyph.minCoordinate.x);
					outputByteStream.write<float32>(glyph.minCoordinate.y);
					outputByteStream.write<float32>(glyph.maxCoordinate.x);
					outputByteStream.write<float32>(glyph.maxCoordinate.y);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.width);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.advanceWidth);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.height);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<sint16>(glyph.leftSideBearing);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<sint16>(glyph.topSideBearing);
				}

				for (const std::pair<uint16, uint32>& unicodeEntry : fontAssetDescriptor.unicodeMap) {
					outputByteStream.write<uint16>(unicodeEntry.first);
				}

				for (const std::pair<uint16, uint32>& unicodeEntry : fontAssetDescriptor.unicodeMap) {
					outputByteStream.write<uint32>(unicodeEntry.second);
				}

				const std::string assetOutputPathString = assetOutputPath.string();

				// Count '\0'.
				sequence<char8> outputPath(assetOutputPathString.size() + std::strlen(DEFAULT_FONT_NAME) + 11);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy_n("/Font/", 6, &outputPath[assetOutputPathString.size()]);
				std::copy_n(DEFAULT_FONT_NAME, std::strlen(DEFAULT_FONT_NAME), &outputPath[assetOutputPathString.size() + 6]);
				std::copy_n(".tfo", 4, &outputPath[assetOutputPathString.size() + std::strlen(DEFAULT_FONT_NAME) + 6]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				ENGINE_INFO("Generated default font asset \"[]\" (ID: []).", outputPath, fontAssetDescriptor.identifier);
			}
	};
}