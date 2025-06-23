#pragma once

namespace ProjectA {
	// File formats supported by the engine for decoding, encoding or both.
	// The structure for a file format T is File<T>.
	enum FileFormat : uint8 {
		// Portable Network Graphics (https://www.w3.org/TR/2003/REC-PNG-20031110).
		PNG = 0,

		// Quite OK Image (https://qoiformat.org).
		QOI = 1,

		// True Type Font (https://developer.apple.com/fonts/TrueType-Reference-Manual).
		TTF = 2,
	};
}