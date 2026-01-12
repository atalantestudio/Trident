#pragma once

#include "Engine/Filesystem/File.hpp"

namespace ProjectA {
	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
	namespace TrueTypeFont {
		// 16.16-bit signed fixed-point number.
		struct Fixed {
			uint16 fraction;
			uint16 integer;
		};

		// 16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
		typedef sint16 FWord;

		// 16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
		typedef uint16 uFWord;

		// 16-bit signed fixed number with the low 14 bits representing fraction.
		typedef sint16 F2Dot14;

		// The long internal format of a date in seconds since 12:00 midnight, January 1, 1904.
		// It is represented as a signed 64-bit integer.
		typedef sint64 longDateTime;

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

	template<>
	struct File<FileFormat::TTF> {
		std::unordered_map<uint16, uint32> unicodeMap;
		sequence<TrueTypeFont::Glyph> glyphs;
		sequence<TrueTypeFont::longHorMetric> hMetrics;
		sequence<TrueTypeFont::FWord> leftSideBearings;
		TrueTypeFont::FWord ascent;
		TrueTypeFont::FWord descent;
		TrueTypeFont::FWord lineGap;
	};
}