#pragma once
#pragma pack(push, 1)

#include "Engine/Filesystem/BitStream.hpp"
#include "Engine/Filesystem/ByteStream.hpp"
#include "Engine/Filesystem/File.hpp"
#include "Engine/Filesystem/HuffmanCodeEntry.hpp"

namespace ProjectA {
	struct Chunk {
		uint32 ByteLength = 0;
		uint32 Type = 0;
		chain<uint8> Data;
		uint32 CRC = 0;
	};

	struct PNG_IHDR {
		// Width of the image in pixels.
		// Zero is an invalid value.
		uint32 Width;

		// Height of the image in pixels.
		// Zero is an invalid value.
		uint32 Height;

		// Number of bytes per sample or per palette index (not per pixel).
		// Valid values are 1, 2, 4, 8, and 16, although not all values are allowed for all color types.
		uint8 BitDepth;

		// PNG image type.
		// Valid values are 0, 2, 3, 4, and 6.
		uint8 ColorType;

		// Method used to compress the image data.
		// Only compression method 0 (deflate/inflate compression with a sliding window of at most 32768 bytes) is defined.
		uint8 CompressionMethod;

		// Preprocessing method applied to the image data before compression.
		// Only filter method 0 (adaptive filtering with five basic filter types) is defined.
		uint8 FilterMethod;

		// Transmission order of the image data.
		// Two values are defined: 0 (no interlace) or 1 (Adam7 interlace).
		uint8 InterlaceMethod;
	};

	// Based on https://www.ietf.org/rfc/rfc1950.txt.
	struct CMF_FLG {
		// Compression method.
		uint8 CM : 4;

		// Compression info.
		uint8 CINFO : 4;

		// Check bits for CMF and FLG.
		uint8 FCHECK : 5;

		// Preset dictionary.
		uint8 FDICT : 1;

		// Compression level.
		uint8 FLEVEL : 2;
	};

	// Based on https://www.w3.org/TR/2003/REC-PNG-20031110.
	class PNGFileLoader {
		public:
			static void load(const std::string& path);

		private:
			static void readSignature(ByteStream& stream);

			static void readChunk(ByteStream& stream);

			static void readIHDR(ByteStream& stream);

			static void readIDAT(ByteStream& stream, uint32 byteLength);

			static void readZlibStream(BitStream& stream);

			static void readZlibBlock(BitStream& stream, uint8& BFINAL, chain<uint8>& output);

			static void readIEND(ByteStream& stream);

		private:
			static constexpr HuffmanCodeEntry lengthExtra[] = {
				{3, 0}, // 257
				{4, 0}, // 258
				{5, 0}, // 259
				{6, 0}, // 260
				{7, 0}, // 261
				{8, 0}, // 262
				{9, 0}, // 263
				{10, 0}, // 264
				{11, 1}, // 265
				{13, 1}, // 266
				{15, 1}, // 267
				{17, 1}, // 268
				{19, 2}, // 269
				{23, 2}, // 270
				{27, 2}, // 271
				{31, 2}, // 272
				{35, 3}, // 273
				{43, 3}, // 274
				{51, 3}, // 275
				{59, 3}, // 276
				{67, 4}, // 277
				{83, 4}, // 278
				{99, 4}, // 279
				{115, 4}, // 280
				{131, 5}, // 281
				{163, 5}, // 282
				{195, 5}, // 283
				{227, 5}, // 284
				{258, 0}, // 285
			};

			static constexpr HuffmanCodeEntry distanceExtra[] = {
				{1, 0}, // 0
				{2, 0}, // 1
				{3, 0}, // 2
				{4, 0}, // 3
				{5, 1}, // 4
				{7, 1}, // 5
				{9, 2}, // 6
				{13, 2}, // 7
				{17, 3}, // 8
				{25, 3}, // 9
				{33, 4}, // 10
				{49, 4}, // 11
				{65, 5}, // 12
				{97, 5}, // 13
				{129, 6}, // 14
				{193, 6}, // 15
				{257, 7}, // 16
				{385, 7}, // 17
				{513, 8}, // 18
				{769, 8}, // 19
				{1025, 9}, // 20
				{1537, 9}, // 21
				{2049, 10}, // 22
				{3073, 10}, // 23
				{4097, 11}, // 24
				{6145, 11}, // 25
				{8193, 12}, // 26
				{12289, 12}, // 27
				{16385, 13}, // 28
				{25577, 13}, // 29
			};
	};
}

#pragma pack(pop)