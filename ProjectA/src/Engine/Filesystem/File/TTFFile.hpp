#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	// 16.16-bit signed fixed-point number.
	struct Fixed {
		uint16 integer;
		uint16 fraction;
	};

	// 16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
	typedef sint16 FWord;

	// 16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
	typedef uint16 uFWord;

	// The long internal format of a date in seconds since 12:00 midnight, January 1, 1904.
	// It is represented as a signed 64-bit integer.
	typedef sint64 longDateTime;

	struct Point {
		FWord x;
		FWord y;
	};

	struct ContourPoint {
		FWord x;
		FWord y;
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
		uint16 codePoint;
		Point min = {0, 0};
		Point max = {0, 0};
		std::vector<Edge> edges;
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

		// Checksum for this table.
		uint32 checksum;

		// Offset from beginning of sfnt.
		uint32 offset;

		// Length of this table in byte (actual length not padded length).
		uint32 length;
	};

	struct maxp {
		// 0x00010000 (1.0).
		Fixed version;

		// The number of glyphs in the font.
		uint16 numGlyphs;

		// Points in non-compound glyph.
		uint16 maxPoints;

		// Contours in non-compound glyph.
		uint16 maxContours;

		// Points in compound glyph.
		uint16 maxComponentPoints;

		// Contours in compound glyph.
		uint16 maxComponentContours;

		// Set to 2.
		uint16 maxZones;

		// Points used in Twilight Zone (Z0).
		uint16 maxTwilightPoints;

		// Number of Storage Area locations.
		uint16 maxStorage;

		// Number of FDEFs.
		uint16 maxFunctionDefs;

		// Number of IDEFs.
		uint16 maxInstructionDefs;

		// Maximum stack depth.
		uint16 maxStackElements;

		// Byte count for glyph instructions.
		uint16 maxSizeOfInstructions;

		// Number of glyphs referenced at top level.
		uint16 maxComponentElements;

		// Levels of recursion, set to 0 if font has only simple glyphs.
		uint16 maxComponentDepth;
	};

	struct hhea {
		// 0x00010000 (1.0).
		Fixed version;

		// Distance from baseline of highest ascender.
		FWord ascent;

		// Distance from baseline of lowest descender.
		FWord descent;

		// Typographic line gap.
		FWord lineGap;

		// Must be consistent with horizontal metrics.
		uFWord advanceWidthMax;

		// Must be consistent with horizontal metrics.
		FWord minLeftSideBearing;

		// Must be consistent with horizontal metrics.
		FWord minRightSideBearing;

		// max(lsb + (xMax - xMin)).
		FWord xMaxExtent;

		// Used to calculate the slope of the caret (rise/run) set to 1 for vertical caret.
		sint16 caretSlopeRise;

		// 0 for vertical.
		sint16 caretSlopeRun;

		// Set value to 0 for non-slanted fonts.
		FWord caretOffset;

		// 0 for current format.
		sint16 metricDataFormat;

		// Number of advance widths in metrics table.
		uint16 numOfLongHorMetrics;
	};

	struct TTFFile : public File {
		std::unordered_map<uint16, uint32> unicodeMap;
		std::vector<Glyph> glyphs;
		sequence<longHorMetric> hMetrics;
		sequence<FWord> leftSideBearings;
		maxp maxp;
		hhea hhea;
	};
}