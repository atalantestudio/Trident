#pragma once

namespace ProjectA {
	class Writer {
		public:
			Writer(std::string_view path) :
				stream(path.data(), std::ios::binary)
			{}

			~Writer() {
				stream.close();
			}

			void write(const sequence<uint8>& data) {
				stream.write(static_cast<const char*>(static_cast<const void*>(data)), data.count);
			}

			void flush() {
				stream.flush();
			}

		private:
			std::ofstream stream;
	};
}