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

	struct EdgePoint {
		FWord x;
		FWord y;
		bool onCurve;
	};

	struct Edge {
		EdgePoint start;
		EdgePoint end;
		EdgePoint control;
		bool isQuadraticCurve;
	};

	struct Glyph {
		Point min;
		Point max;
		chain<Edge> edges;
	};

	struct TTFFile : public File {
		fchain<Glyph> glyphs;
	};
}