// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"

#include "AssetBuilder/Process/Font/FontProcess.hpp"
#include "../../../File/PNG.hpp"

namespace Trident {
	class DefaultFont : public FontProcess {
		private:
			struct BitmapGlyph {
				const char8* codePoint;
				vec2<float32> minCoordinate;
				vec2<uint16> size;
				sint16 topSideBearing;
			};

		private:
			static constexpr sint16 ASCENT = 9;
			static constexpr sint16 DESCENT = -3;
			static constexpr sint16 LINE_GAP = 0;
			static constexpr uint16 UNITS_PER_EM = 10;
			static constexpr index BITMAP_GLYPH_COUNT = 144;
			static constexpr BitmapGlyph BITMAP_GLYPHS[BITMAP_GLYPH_COUNT] = {
				{"A", {0.0f, 0.0f}, {5, 7}, 2},
				{"B", {6.0f, 0.0f}, {5, 7}, 2},
				{"C", {12.0f, 0.0f}, {5, 7}, 2},
				{"D", {18.0f, 0.0f}, {5, 7}, 2},
				{"E", {24.0f, 0.0f}, {5, 7}, 2},
				{"F", {30.0f, 0.0f}, {5, 7}, 2},
				{"G", {36.0f, 0.0f}, {5, 7}, 2},
				{"H", {42.0f, 0.0f}, {5, 7}, 2},
				{"I", {48.0f, 0.0f}, {3, 7}, 2},
				{"J", {52.0f, 0.0f}, {5, 8}, 2},
				{"K", {58.0f, 0.0f}, {5, 7}, 2},
				{"L", {64.0f, 0.0f}, {5, 7}, 2},
				{"M", {70.0f, 0.0f}, {5, 7}, 2},
				{"N", {76.0f, 0.0f}, {5, 7}, 2},
				{"O", {82.0f, 0.0f}, {5, 7}, 2},
				{"P", {88.0f, 0.0f}, {5, 7}, 2},
				{"Q", {94.0f, 0.0f}, {5, 8}, 2},
				{"R", {100.0f, 0.0f}, {5, 7}, 2},
				{"S", {106.0f, 0.0f}, {5, 7}, 2},
				{"T", {112.0f, 0.0f}, {5, 7}, 2},
				{"U", {118.0f, 0.0f}, {5, 7}, 2},
				{"V", {0.0f, 12.0f}, {5, 7}, 2},
				{"W", {6.0f, 12.0f}, {5, 7}, 2},
				{"X", {12.0f, 12.0f}, {5, 7}, 2},
				{"Y", {18.0f, 12.0f}, {5, 7}, 2},
				{"Z", {24.0f, 12.0f}, {5, 7}, 2},
				{"a", {30.0f, 12.0f}, {5, 5}, 4},
				{"b", {36.0f, 12.0f}, {5, 7}, 2},
				{"c", {42.0f, 12.0f}, {5, 7}, 4},
				{"d", {48.0f, 12.0f}, {5, 7}, 2},
				{"e", {54.0f, 12.0f}, {5, 7}, 4},
				{"f", {60.0f, 12.0f}, {4, 12}, 0},
				{"g", {65.0f, 12.0f}, {5, 8}, 4},
				{"h", {71.0f, 12.0f}, {5, 7}, 2},
				{"i", {77.0f, 12.0f}, {1, 7}, 2},
				{"j", {79.0f, 12.0f}, {4, 9}, 2},
				{"k", {84.0f, 12.0f}, {4, 7}, 2},
				{"l", {89.0f, 12.0f}, {3, 7}, 2},
				{"m", {93.0f, 12.0f}, {5, 5}, 4},
				{"n", {99.0f, 12.0f}, {5, 5}, 4},
				{"o", {105.0f, 12.0f}, {5, 5}, 4},
				{"p", {111.0f, 12.0f}, {5, 7}, 4},
				{"q", {117.0f, 12.0f}, {5, 7}, 4},
				{"r", {0.0f, 24.0f}, {5, 5}, 4},
				{"s", {6.0f, 24.0f}, {5, 5}, 4},
				{"t", {12.0f, 24.0f}, {3, 7}, 2},
				{"u", {16.0f, 24.0f}, {5, 5}, 4},
				{"v", {22.0f, 24.0f}, {5, 5}, 4},
				{"w", {28.0f, 24.0f}, {5, 5}, 4},
				{"x", {34.0f, 24.0f}, {5, 5}, 4},
				{"y", {40.0f, 24.0f}, {5, 8}, 4},
				{"z", {46.0f, 24.0f}, {5, 8}, 4},
				{"0", {52.0f, 24.0f}, {5, 7}, 2},
				{"1", {58.0f, 24.0f}, {3, 7}, 2},
				{"2", {62.0f, 24.0f}, {5, 7}, 2},
				{"3", {68.0f, 24.0f}, {5, 7}, 2},
				{"4", {74.0f, 24.0f}, {5, 7}, 2},
				{"5", {80.0f, 24.0f}, {5, 7}, 2},
				{"6", {86.0f, 24.0f}, {5, 7}, 2},
				{"7", {92.0f, 24.0f}, {5, 7}, 2},
				{"8", {98.0f, 24.0f}, {5, 7}, 2},
				{"9", {104.0f, 24.0f}, {5, 7}, 2},
				{".", {110.0f, 24.0f}, {1, 1}, 8},
				{",", {112.0f, 24.0f}, {2, 3}, 8},
				{"“", {115.0f, 24.0f}, {4, 3}, 2},
				{"”", {120.0f, 24.0f}, {4, 3}, 6},
				{"‘", {0.0f, 36.0f}, {2, 3}, 2},
				{"’", {3.0f, 36.0f}, {2, 3}, 2},
				{"\"", {6.0f, 36.0f}, {3, 2}, 2},
				{"'", {10.0f, 36.0f}, {1, 2}, 2},
				{"?", {12.0f, 36.0f}, {4, 7}, 2},
				{"!", {17.0f, 36.0f}, {1, 7}, 2},
				{"@", {19.0f, 36.0f}, {7, 7}, 2},
				{"_", {27.0f, 36.0f}, {5, 1}, 8},
				{"*", {33.0f, 36.0f}, {3, 3}, 2},
				{"#", {37.0f, 36.0f}, {5, 7}, 2},
				{"$", {43.0f, 36.0f}, {5, 7}, 2},
				{"%", {49.0f, 36.0f}, {5, 7}, 2},
				{"&", {55.0f, 36.0f}, {5, 7}, 2},
				{"(", {61.0f, 36.0f}, {2, 9}, 1},
				{")", {64.0f, 36.0f}, {2, 9}, 1},
				{"+", {67.0f, 36.0f}, {5, 5}, 4},
				{"-", {73.0f, 36.0f}, {5, 1}, 6},
				{"/", {79.0f, 36.0f}, {5, 7}, 2},
				{":", {85.0f, 36.0f}, {1, 5}, 4},
				{";", {87.0f, 36.0f}, {2, 7}, 4},
				{"<", {90.0f, 36.0f}, {5, 5}, 4},
				{"=", {96.0f, 36.0f}, {5, 3}, 5},
				{">", {102.0f, 36.0f}, {5, 5}, 4},
				{"[", {108.0f, 36.0f}, {2, 9}, 1},
				{"\\", {111.0f, 36.0f}, {5, 7}, 2},
				{"]", {117.0f, 36.0f}, {2, 9}, 1},
				{"^", {120.0f, 36.0f}, {3, 2}, 1},
				{"`", {124.0f, 36.0f}, {2, 2}, 1},
				{"{", {0.0f, 48.0f}, {3, 9}, 1},
				{"|", {4.0f, 48.0f}, {1, 9}, 1},
				{"}", {6.0f, 48.0f}, {3, 9}, 1},
				{"~", {10.0f, 48.0f}, {5, 3}, 5},
				{"¡", {16.0f, 48.0f}, {1, 7}, 2},
				{"¦", {18.0f, 48.0f}, {1, 9}, 1},
				{"§", {20.0f, 48.0f}, {5, 7}, 2},
				{"¨", {26.0f, 48.0f}, {3, 1}, 2},
				{"©", {30.0f, 48.0f}, {7, 7}, 2},
				{"«", {38.0f, 48.0f}, {5, 3}, 5},
				{"°", {44.0f, 48.0f}, {3, 3}, 3},
				{"±", {48.0f, 48.0f}, {5, 5}, 4},
				{"»", {54.0f, 48.0f}, {5, 3}, 5},
				{"¿", {60.0f, 48.0f}, {4, 7}, 2},
				{"Æ", {65.0f, 48.0f}, {7, 7}, 2},
				{"×", {73.0f, 48.0f}, {5, 5}, 4},
				{"Ø", {79.0f, 48.0f}, {7, 7}, 2},
				{"à", {87.0f, 48.0f}, {5, 8}, 1},
				{"á", {93.0f, 48.0f}, {5, 8}, 1},
				{"â", {99.0f, 48.0f}, {5, 8}, 1},
				{"ã", {105.0f, 48.0f}, {5, 8}, 1},
				{"ä", {111.0f, 48.0f}, {5, 7}, 2},
				{"\xC8\xA7", {117.0f, 48.0f}, {5, 7}, 2}, // 'ȧ'
				{"æ", {0.0f, 60.0f}, {7, 5}, 4},
				{"ç", {8.0f, 60.0f}, {5, 7}, 4},
				{"è", {14.0f, 60.0f}, {5, 8}, 1},
				{"é", {20.0f, 60.0f}, {5, 8}, 1},
				{"ê", {26.0f, 60.0f}, {5, 8}, 1},
				{"ë", {32.0f, 60.0f}, {5, 7}, 2},
				{"ì", {38.0f, 60.0f}, {2, 8}, 1},
				{"í", {41.0f, 60.0f}, {2, 8}, 1},
				{"î", {44.0f, 60.0f}, {3, 8}, 1},
				{"ï", {48.0f, 60.0f}, {3, 7}, 2},
				{"ñ", {52.0f, 60.0f}, {5, 8}, 1},
				{"ò", {58.0f, 60.0f}, {5, 8}, 1},
				{"ó", {64.0f, 60.0f}, {5, 8}, 1},
				{"ô", {70.0f, 60.0f}, {5, 8}, 1},
				{"õ", {76.0f, 60.0f}, {5, 8}, 1},
				{"ö", {82.0f, 60.0f}, {5, 7}, 2},
				{"÷", {88.0f, 60.0f}, {5, 5}, 4},
				{"ø", {94.0f, 60.0f}, {5, 5}, 4},
				{"ù", {100.0f, 60.0f}, {5, 8}, 1},
				{"ú", {106.0f, 60.0f}, {5, 8}, 1},
				{"û", {112.0f, 60.0f}, {5, 8}, 1},
				{"ü", {118.0f, 60.0f}, {5, 7}, 2},
				{"Œ", {0.0f, 72.0f}, {7, 7}, 2},
				{"œ", {8.0f, 72.0f}, {7, 5}, 4},
				{"€", {16.0f, 72.0f}, {5, 7}, 2},
				{"™", {22.0f, 72.0f}, {6, 3}, 3},
				{" ", {29.0f, 72.0f}, {4, 0}, 0},
			};

		protected:
			view<char8> getAssetName() const final override {
				return "default";
			}

			BuildFont createFont(AssetBuilderLibrary* assetLibrary) const final override {
				const ref<ImageAsset> glyphMapImage = assetLibrary->getAsset<ImageAsset>("Quiver");

				DEBUG_ASSERT(glyphMapImage->format == ImageFormat::R8_UNORM);

				BuildFont font = {
					.glyphMapImage = glyphMapImage,
					.ascent = ASCENT,
					.descent = DESCENT,
					.lineGap = LINE_GAP,
					.unitsPerEm = UNITS_PER_EM,
					.glyphs = sequence<Glyph>(BITMAP_GLYPH_COUNT),
					.unicodeMap = {},
					.kerningPairs = {},
				};

				uint32 glyphIndex = 0;

				while (glyphIndex < font.glyphs.count()) {
					const BitmapGlyph& bitmapGlyph = BITMAP_GLYPHS[glyphIndex];
					const UTF8Stream stream = UTF8Stream(bitmapGlyph.codePoint);
					const uint32 codePoint = stream.peek();

					Glyph& glyph = font.glyphs[glyphIndex];
					glyph.minCoordinate.x = bitmapGlyph.minCoordinate.x / glyphMapImage->extent.x;
					glyph.minCoordinate.y = (bitmapGlyph.minCoordinate.y + bitmapGlyph.topSideBearing) / glyphMapImage->extent.y;
					glyph.maxCoordinate.x = (bitmapGlyph.minCoordinate.x + bitmapGlyph.size.x) / glyphMapImage->extent.x;
					glyph.maxCoordinate.y = (bitmapGlyph.minCoordinate.y + bitmapGlyph.size.y + bitmapGlyph.topSideBearing) / glyphMapImage->extent.y;
					glyph.size = bitmapGlyph.size;
					glyph.yMax = ASCENT - bitmapGlyph.topSideBearing;
					glyph.advanceWidth = bitmapGlyph.size.x + 1;
					glyph.leftSideBearing = 0;
					glyph.whitespace = codePoint == ' ';

					font.unicodeMap.insert({static_cast<uint16>(codePoint), glyphIndex});

					glyphIndex += 1;
				}

				return font;
			}

		public:
			using FontProcess::FontProcess;
	};
}