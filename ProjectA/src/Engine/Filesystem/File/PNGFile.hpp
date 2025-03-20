#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	struct PNGFile : public File {
		// Width of the image in pixels.
		// Zero is an invalid value.
		uint32 width;

		// Height of the image in pixels.
		// Zero is an invalid value.
		uint32 height;

		// Number of bytes per sample or per palette index (not per pixel).
		// Valid values are 1, 2, 4, 8, and 16, although not all values are allowed for all color types.
		uint8 bitDepth;

		// PNG image type.
		// Valid values are 0, 2, 3, 4, and 6.
		uint8 colorType;

		// Method used to compress the image data.
		// Only compression method 0 (deflate/inflate compression with a sliding window of at most 32768 bytes) is defined.
		uint8 compressionMethod;

		// Preprocessing method applied to the image data before compression.
		// Only filter method 0 (adaptive filtering with five basic filter types) is defined.
		uint8 filterMethod;

		// Transmission order of the image data.
		// Two values are defined: 0 (no interlace) or 1 (Adam7 interlace).
		uint8 interlaceMethod;

		// Compressed data (mix of all IDAT chunks).
		chain<uint8> encodedData;

		// Decompressed data without PNG filters applied.
		chain<uint8> unfilteredData;

		// Decompressed data with PNG filters applied.
		chain<uint8> data;
	};
}