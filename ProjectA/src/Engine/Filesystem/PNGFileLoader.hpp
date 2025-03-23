#pragma once

#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	struct Chunk {
		uint32 ByteLength = 0;
		uint32 Type = 0;
		chain<uint8> Data;
		uint32 CRC = 0;
	};

	// Based on https://www.w3.org/TR/2003/REC-PNG-20031110.
	class PNGFileLoader {
		public:
			static PNGFile load(const std::string& path);

		private:
			static void readSignature(ByteStream& stream);

			static void readChunk(ByteStream& stream, PNGFile& file);

			static void readIHDRChunk(ByteStream& stream, PNGFile& file);

			static void readIDATChunk(ByteStream& stream, uint32 chunkByteLength, PNGFile& file);

			static uint8 applyAddFilter(const uint8* x, uint8* aOrB, uint8 channelIndex);
			static uint8 applyAverageFilter(const uint8* x, uint8* a, uint8* b, uint8 channelIndex);
			static uint8 applyPaethFilter(const uint8* x, uint8* a, uint8* b, uint8* c, uint8 channelIndex);
	};
}