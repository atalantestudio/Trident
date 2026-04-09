// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<StreamOperation StreamOperation>
	class BitStream;

	template<>
	class BitStream<StreamOperation::READ> {
		private:
			typedef uint64 BitStreamSize;

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = 8 * sizeof(BitStreamSize);

		public:
			explicit BitStream(const uint8* data, uint64 byteSize, Endianness endianness) :
				data(data),
				bitBuffer(*pointer_cast<const BitStreamSize*>(&data[0])),
				byteSize(byteSize),
				bitOffset(0),
				endianness(endianness)
			{}

			// TODO: Move to a new parent class.
			uint64 getByteOffset() const {
				return byteOffset;
			}

			uint64 getBitOffset() const {
				return byteOffset * 8 + bitOffset;
			}

			const uint8* getBytePosition() const {
				return data + byteOffset + bitOffset / 8;
			}

			// Returns the remaining bit size.
			uint64 getRemainingBitSize() const {
				return (byteSize - byteOffset) * 8 - bitOffset;
			}

			Endianness getEndianness() const {
				return endianness;
			}

			// Advances the offsets by the provided bit count.
			// The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount) {
				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to jump [] bits, but only [] were remaining.", bitCount, remainingBitSize);

				uint8 jumpBitCount = 0;

				if (bitOffset + bitCount >= sizeof(BitStreamSize) * 8) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(bitBuffer) * 8);

					bitOffset = 0;
					byteOffset += sizeof(bitBuffer) * jumpByteCount;
					bitBuffer = *(BitStreamSize*)(data + byteOffset);
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(bitBuffer) * 8);

				bitOffset += jumpBitCount;
				bitBuffer >>= jumpBitCount;
			}

			// Returns the next bits from the stream without advancing the internal offset.
			// The bit count must be <= the number of bits in T.
			// The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T peek(uint8 bitCount, bool swapEndianness = false) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "Cannot peek [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "Cannot peek more than [] bits at once.", static_cast<sint32>(MAX_READ_OR_PEEK_BIT_COUNT));

				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to peek [] bits, but only [] were remaining.", static_cast<sint32>(bitCount), remainingBitSize);

				T value = *pointer_cast<const T*>(&bitBuffer);

				if (static_cast<uint8>(endianness) + swapEndianness ^ static_cast<uint8>(Platform::getEndianness())) {
					value = byteswap(value);
				}

				BitStreamSize valueStreamSize = *pointer_cast<const BitStreamSize*>(&value);

				if (bitOffset > 0) {
					const BitStreamSize offset = sizeof(bitBuffer) * 8 - bitOffset;

					valueStreamSize |= (*((BitStreamSize*) (data + byteOffset + sizeof(bitBuffer)))) << offset;
				}

				// Create a mask from the bit count.
				const BitStreamSize bitCountMask = (static_cast<BitStreamSize>(1) << bitCount) - 1;

				valueStreamSize &= bitCountMask;

				return *pointer_cast<const T*>(&valueStreamSize);
			}

			// Returns the next bits from the stream.
			// The bit count must be <= the number of bits in T.
			// The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T read(uint8 bitCount, bool swapEndianness = false) {
				const T value = peek<T>(bitCount, swapEndianness);

				jump(bitCount);

				return value;
			}

			// TODO: Implement seek<T>(uint8 bitCount).

			// Uncompresses data using the DEFLATE decompression algorithm.
			// `uncompressedSize` is multiplied by the size of T.
			template<typename T>
			sequence<T> inflate(uint64 uncompressedSize) {
				// CMF (Compression Method and flags)
				// 
				// This byte is divided into a 4-bit compression method and a 4-
				// bit information field depending on the compression method.
				// 
				//    bits 0 to 3  CM     Compression method
				//    bits 4 to 7  CINFO  Compression info
				// 
				const uint8 CMF = read<uint8>(8);

				// CM (Compression method)
				// 
				// This identifies the compression method used in the file. CM = 8
				// denotes the "deflate" compression method with a window size up
				// to 32K. This is the method used by gzip and PNG (see
				// references [1] and [2] in Chapter 3, below, for the reference
				// documents). CM = 15 is reserved. It might be used in a future
				// version of this specification to indicate the presence of an
				// extra field before the compressed data.
				const uint8 CM = CMF & 0b00001111;

				ENGINE_ASSERT(CM != 15, "CM = 15 is a reserved value. See https://datatracker.ietf.org/doc/html/rfc1950.");
				ENGINE_ASSERT(CM == 8, "CM = [] is an unhandled value.", static_cast<sint32>(CM));

				// CINFO (Compression info)
				// 
				// For CM = 8, CINFO is the base-2 logarithm of the LZ77 window
				// size, minus eight (CINFO=7 indicates a 32K window size). Values
				// of CINFO above 7 are not allowed in this version of the
				// specification. CINFO is not defined in this specification for
				// CM not equal to 8.
				const uint8 CINFO = CMF >> 4;

				ENGINE_ASSERT(CINFO <= 7, "CINFO = [] is not allowed. See https://datatracker.ietf.org/doc/html/rfc1950.", static_cast<sint32>(CINFO));

				// FLG (FLaGs)
				// 
				// This flag byte is divided as follows:
				// 
				//    bits 0 to 4  FCHECK  (check bits for CMF and FLG)
				//    bit  5       FDICT   (preset dictionary)
				//    bits 6 to 7  FLEVEL  (compression level)
				// 
				const uint8 FLG = read<uint8>(8);

				// FCHECK (Check bits for CMF and FLG)
				// 
				// The FCHECK value must be such that CMF and FLG, when viewed as
				// a 16-bit unsigned integer stored in MSB order (CMF * 256 + FLG),
				// is a multiple of 31.
				const uint8 FCHECK = FLG & 0b00011111;

				ENGINE_ASSERT((CMF * 256 + FLG) % 31 == 0, "FCHECK check failed. See https://datatracker.ietf.org/doc/html/rfc1950.");

				// FDICT (Preset dictionary)
				// 
				// If FDICT is set, a DICT dictionary identifier is present
				// immediately after the FLG byte. The dictionary is a sequence of
				// bytes which are initially fed to the compressor without
				// producing any compressed output. DICT is the Adler-32 checksum
				// of this sequence of bytes (see the definition of ADLER32
				// below). The decompressor can use this identifier to determine
				// which dictionary has been used by the compressor.
				const uint8 FDICT = (FLG >> 5) & 0b00000001;

				ENGINE_ASSERT(FDICT == 0, "FDICT = [] is an unhandled value.", static_cast<sint32>(FDICT));

				// FLEVEL (Compression level)
				// 
				// These flags are available for use by specific compression
				// methods. The "deflate" method (CM = 8) sets these flags as
				// follows:
				// 
				//    0 - compressor used fastest algorithm
				//    1 - compressor used fast algorithm
				//    2 - compressor used default algorithm
				//    3 - compressor used maximum compression, slowest algorithm
				// 
				// The information in FLEVEL is not needed for decompression; it
				// is there to indicate if recompression might be worthwhile.
				const uint8 FLEVEL = FLG >> 6;

				sequence<T> data(uncompressedSize);
				uint64 dataOffset = 0;

				// TODO: Compute theoretical max. number of blocks.
				static constexpr uint32 MAX_BLOCK_COUNT = 1000;

				for (uint32 blockIndex = 0; blockIndex < MAX_BLOCK_COUNT; blockIndex += 1) {
					// BFINAL is set if and only if this is the last block of the data set.
					const bool BFINAL = read<uint8>(1);

					// BTYPE specifies how the data are compressed, as follows:
					// 
					//    00 - no compression
					//    01 - compressed with fixed Huffman codes
					//    10 - compressed with dynamic Huffman codes
					//    11 - reserved (error)
					// 
					// The only difference between the two compressed cases is how the
					// Huffman codes for the literal/length and distance alphabets are
					// defined.
					const uint8 BTYPE = read<uint8>(2);

					ENGINE_ASSERT(BTYPE != 0b11, "BTYPE = 0b11 is a reserved value. See https://datatracker.ietf.org/doc/html/rfc1951.");

					if (BTYPE == 0) {
						parseUncompressedZlibBlock(pointer_cast<uint8*>(&data[0]));
					}
					else {
						parseCompressedZlibBlock(pointer_cast<uint8*>(&data[0]), dataOffset, BTYPE);
					}

					if (BFINAL) {
						// TODO: Check Adler32.
						return data;
					}
				}

				ENGINE_WARNING("Reached last safe iteration.");

				return data;
			}

		private:
			void parseUncompressedZlibBlock(uint8* data);

			void parseCompressedZlibBlock(uint8* data, uint64& dataOffset, uint8 BTYPE);

		private:
			// TODO: Should stream be BitStreamSize* instead?
			const uint8* data;

			// Bit accumulator.
			// Missed opportunity to name this "bitBucket".
			BitStreamSize bitBuffer = 0;

			// Size of the stream in bytes.
			uint64 byteSize;

			// Offset of byte that is currently being read.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			uint8 bitOffset: 6;

			Endianness endianness;
	};

	// TODO: Remove duplicated methods.
	template<>
	class BitStream<StreamOperation::WRITE> {
		private:
			typedef uint64 BitStreamSize;

		private:
			static constexpr uint8 MAX_WRITE_BIT_COUNT = 8 * sizeof(BitStreamSize);

		public:
			explicit BitStream(uint8* data, uint64 byteSize, Endianness endianness) :
				data(data),
				byteSize(byteSize),
				bitOffset(0),
				endianness(endianness)
			{}

			// TODO: Move to a new parent class.
			uint64 getByteOffset() const {
				return byteOffset;
			}

			uint64 getBitOffset() const {
				return byteOffset * 8 + bitOffset;
			}

			const uint8* getBytePosition() const {
				return data + byteOffset + bitOffset / 8;
			}

			// Returns the remaining bit size.
			uint64 getRemainingBitSize() const {
				return (byteSize - byteOffset) * 8 - bitOffset;
			}

			Endianness getEndianness() const {
				return endianness;
			}

			// Advances the offsets by the provided bit count.
			// The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount) {
				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to jump [] bits, but only [] were remaining.", bitCount, remainingBitSize);

				uint8 jumpBitCount = 0;

				if (bitOffset + bitCount >= sizeof(BitStreamSize) * 8) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(BitStreamSize) * 8);

					bitOffset = 0;
					byteOffset += sizeof(BitStreamSize) * jumpByteCount;
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(BitStreamSize) * 8);

				bitOffset += jumpBitCount;
			}

			// TODO: Implement seek<T>(uint8 bitCount).

			// Writes `bitCount` bits of `value` to the stream.
			template<typename T>
			void write(T value, uint8 bitCount) {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "The bit count ([]) cannot be greater than the number of bits in the value ([]).", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_WRITE_BIT_COUNT, "Cannot write more than [] bits at once.", static_cast<sint32>(MAX_WRITE_BIT_COUNT));

				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to write [] bits, but only [] were remaining.", static_cast<sint32>(bitCount), remainingBitSize);

				// Create a mask from the bit count.
				const T bitCountMask = (static_cast<T>(1) << bitCount) - 1;

				const BitStreamSize partToBeUpdated = static_cast<BitStreamSize>(bitCountMask) << bitOffset;

				BitStreamSize* bitBuffer = pointer_cast<BitStreamSize*>(data + byteOffset);

				bitBuffer[0] |= static_cast<BitStreamSize>(value & bitCountMask) << bitOffset;

				if (bitOffset + bitCount >= sizeof(BitStreamSize) * 8) {
					// If the value overflows the offsetted bit buffer, continue the write beyond the bit buffer.
					const BitStreamSize offset = sizeof(BitStreamSize) * 8 - bitOffset;

					bitBuffer[1] |= static_cast<BitStreamSize>(value & bitCountMask) >> offset;
				}

				jump(bitCount);
			}

		private:
			// TODO: Should stream be BitStreamSize* instead?
			uint8* data;

			// Size of the stream in bytes.
			uint64 byteSize;

			// Offset of byte that is currently being read.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			uint8 bitOffset: 6;

			Endianness endianness;
	};
}