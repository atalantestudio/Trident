// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/File.hpp"
#include "Engine/Font/Glyph.hpp"

#include "File/TTF/Types.hpp"
#include "File/TTF/GPOS.hpp"

namespace Trident {
	namespace TrueTypeFont {
		struct OffsetSubtable {
			// A tag to indicate the OFA scaler to be used to rasterize this font.
			uint32 scalerType;

			// Number of tables.
			uint16 numTables;

			// (maximum power of 2 <= numTables) * 16.
			uint16 searchRange;

			// log2(maximum power of 2 <= numTables).
			uint16 entrySelector;

			// numTables * 16 - searchRange.
			uint16 rangeShift;
		};

		struct TableDirectory {
			// 4-byte identifier.
			uint32 tag;

			// Offset from beginning of sfnt.
			uint32 offset;

			// Length of this table in byte (actual length not padded length).
			uint32 length;
		};

		struct Point {
			FWord x;
			FWord y;
		};

		struct ContourPoint : public Point {
			bool onCurve;
		};

		struct Edge {
			Point start;
			Point end;
			Point control;
			bool curved;
		};

		struct longHorMetric {
			uint16 advanceWidth;
			sint16 leftSideBearing;
		};

		struct Glyph {
			Point min;
			Point max;
			sequence<Edge> edges;

			bool emptyOrUnparsed() const {
				return edges.count() == 0;
			}
		};

		struct GlyphTransform {
			float32 a = 1.0f;
			float32 b = 0.0f;
			float32 c = 0.0f;
			float32 d = 1.0f;
			float32 e = 0.0f;
			float32 f = 0.0f;
			float32 m = 0.0f;
			float32 n = 0.0f;
		};
	}
}

namespace Trident {
	template<>
	struct File<FileFormat::TTF> {
		std::unordered_map<uint16, uint32> unicodeMap;
		sequence<TrueTypeFont::Glyph> glyphs;
		sequence<TrueTypeFont::longHorMetric> hMetrics;
		sequence<TrueTypeFont::FWord> leftSideBearings;

		/// The value is the X advance of the first glyph.
		std::unordered_map<GlyphPair, sint16> kerningPairs;

		TrueTypeFont::FWord ascent;
		TrueTypeFont::FWord descent;
		TrueTypeFont::FWord lineGap;
	};
}