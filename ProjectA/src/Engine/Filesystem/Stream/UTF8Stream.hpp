#pragma once

#include "Engine/Filesystem/Stream/Stream.hpp"

namespace ProjectA {
	// A read-only stream containing UTF-8 characters.
	class UTF8Stream {
		public:
			UTF8Stream() = default;

			explicit UTF8Stream(view<char8> data) :
				data(pointer_cast<const uint8*>(&data[0])),
				byteSize(data.count())
			{}

			inline const uint8* getData() const {
				return data;
			}

			inline uint64 getByteSize() const {
				return byteSize;
			}

			inline uint64 getByteOffset() const {
				return byteOffset;
			}

			inline uint64 getRemainingByteSize() const {
				return byteSize - byteOffset;
			}

			// Advances the internal byte offset over the code point represented by the next byte(s).
			// If this code point is invalid, the offset is increased by 1.
			inline void jump() {
				const uint8 codePointByteSize = extractCodePointByteSize();

				ENGINE_ASSERT(byteOffset + codePointByteSize <= byteSize, "Attempted to jump [] bytes, but only [] were remaining.", codePointByteSize, byteSize - byteOffset);

				byteOffset += codePointByteSize;
			}

			// Advances the internal byte offset relatively to the start of the stream.
			inline void seek(uint64 byteOffset) {
				ENGINE_ASSERT(byteOffset <= byteSize, "Cannot seek more bytes than there is in the stream.", byteOffset, byteSize);

				this->byteOffset = byteOffset;
			}

			// Returns the code point represented by the next byte(s) without advancing the internal offset.
			inline uint32 peek() const {
				const uint8 codePointByteSize = extractCodePointByteSize();

				ENGINE_ASSERT(byteOffset + codePointByteSize <= byteSize, "Attempted to peek [] bytes, but only [] were remaining.", codePointByteSize, byteSize - byteOffset);

				const uint32 codePoint = extractCodePoint(codePointByteSize);

				accumulateError(codePoint, codePointByteSize);

				return codePoint;
			}

			// See https://nullprogram.com/blog/2017/10/06.
			// Returns the code point represented by the next byte(s), then advances the internal byte offset.
			inline uint32 read() {
				const uint8 codePointByteSize = extractCodePointByteSize();

				ENGINE_ASSERT(byteOffset + codePointByteSize <= byteSize, "Attempted to read [] bytes, but only [] were remaining.", codePointByteSize, byteSize - byteOffset);

				const uint32 codePoint = extractCodePoint(codePointByteSize);

				accumulateError(codePoint, codePointByteSize);

				byteOffset += codePointByteSize + !codePointByteSize;

				return codePoint;
			}

		private:
			// See https://nullprogram.com/blog/2017/10/06.
			inline uint8 extractCodePointByteSize() const {
				// Code point byte sizes depending on the first byte prefix.
				static constexpr uint8 CODE_POINT_BYTE_SIZES[] = {
					// [0[
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

					// [10[ (invalid)
					0, 0, 0, 0, 0, 0, 0, 0,

					// [110[
					2, 2, 2, 2,

					// [1110[
					3, 3,

					// [11110[
					4,

					// [11111[ (invalid)
					0,
				};

				ENGINE_ASSERT(byteOffset < byteSize, "No bytes remaining.");

				return CODE_POINT_BYTE_SIZES[data[byteOffset] >> 3];
			}

			// See https://nullprogram.com/blog/2017/10/06.
			inline uint32 extractCodePoint(uint8 codePointByteSize) const {
				// Leading byte masks per code point byte size, representing the actual data.
				static constexpr uint8 LEADING_BYTE_DATA_MASKS[] = {0, 0b01111111, 0b00011111, 0b00001111, 0b00000111};

				// Shift values per code point byte size, used to discard the unneeded bits.
				static constexpr uint8 CODE_POINT_SHIFTS[] = {0, 18, 12, 6, 0};

				uint32 codePoint = 0;

				codePoint |= static_cast<uint32>(data[byteOffset + 0] & LEADING_BYTE_DATA_MASKS[codePointByteSize]) << 18;
				codePoint |= static_cast<uint32>(data[byteOffset + 1] & 0b00111111) << 12;
				codePoint |= static_cast<uint32>(data[byteOffset + 2] & 0b00111111) << 6;
				codePoint |= static_cast<uint32>(data[byteOffset + 3] & 0b00111111) << 0;
				codePoint >>= CODE_POINT_SHIFTS[codePointByteSize];

				return codePoint;
			}

			// See https://nullprogram.com/blog/2017/10/06.
			inline void accumulateError(uint32 codePoint, uint8 codePointByteSize) const {
				// Minimum code point values per code point byte size.
				// NOTE: 4194304 is 1 + the largest 22-bit number (a 4 byte code point can contain up to 22 bits of data). It is used to handle invalid code points.
				static constexpr uint32 MINIMUM_CODE_POINTS[] = {4194304, 0, 128, 2048, 65536};

				// Shift values per code point byte size, used to discard the unneeded bits in the error value.
				static constexpr uint8 ERROR_SHIFTS[] = {0, 6, 4, 2, 0};

				uint32 error = 0;

				// Check if the shortest encoding was used.
				error = (codePoint < MINIMUM_CODE_POINTS[codePointByteSize]) << 6;

				// Validate the highest 2 bits of the 3 last bytes.
				error |= (data[byteOffset + 1] & 0b11000000) >> 2;
				error |= (data[byteOffset + 2] & 0b11000000) >> 4;
				error |= data[byteOffset + 3] >> 6;
				error ^= 0b00101010;
				error >>= ERROR_SHIFTS[codePointByteSize];

				DEBUG_ASSERT(error == 0);
			}

		private:
			const uint8* data = nullptr;
			uint64 byteSize = 0;
			uint64 byteOffset = 0;
	};
}