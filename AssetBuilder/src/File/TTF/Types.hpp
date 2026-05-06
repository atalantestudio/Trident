// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html.
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

		typedef uint32 Tag;
		typedef uint16 Offset16;
		typedef uint32 Offset32;
	}
}