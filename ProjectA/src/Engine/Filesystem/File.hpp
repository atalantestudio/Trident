#pragma once

namespace ProjectA {
	class File {
		public:
			static void swapEndianness(uint16* value);
			static void swapEndianness(uint32* value);

		public:
			File(const std::string& path);

			~File();

			std::streampos getPosition();

			bool eof();

			void advance(std::streamsize byteCount);

			template<typename T>
			T read() {
				T value{};

				stream.read(static_cast<char*>(static_cast<void*>(&value)), sizeof(T));

				return value;
			}

			// TODO: Review.
			uint32 readBits(uint32 bitCount) {
				const uint32 value = peekBits(bitCount);

				discardBits(bitCount);

				return value;
			}

			// TODO: Review.
			uint32 peekBits(uint32 bitCount) {
				uint32 value = 0;

				while (remainingBitBufferCount < bitCount) {
					const uint32 byte = read<uint8>();

					bitBuffer |= (byte << remainingBitBufferCount);
					remainingBitBufferCount += 8;
				}

				value = bitBuffer & ((1 << bitCount) - 1);
				//const bool bitUnderflow = (remainingBitBufferCount < bitCount);

				return value;
			}

			void discardBits(uint32 bitCount) {
				remainingBitBufferCount -= bitCount;
				bitBuffer >>= bitCount;
			}

			// NOTE: A count of 0 is valid for some formats.
			template<typename T>
			chain<T> read(uint32 count) {
				chain<T> chain(count);

				stream.read(static_cast<char*>(static_cast<void*>(chain.data())), count);

				return chain;
			}

			// TODO: void inflate()

		private:
			std::ifstream stream;
			uint8 bitBuffer = 0;
			uint8 remainingBitBufferCount = 0;
	};
}