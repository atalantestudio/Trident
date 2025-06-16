#pragma once

namespace ProjectA {
	class FileStream {
		public:
			FileStream(std::string_view path) :
				stream(path.data(), std::ios::binary)
			{}

			void write(const sequence<uint8>& data) {
				stream.write(static_cast<const char*>(static_cast<const void*>(data)), data.count);
			}

			void flush() {
				stream.flush();
			}

			~FileStream() {
				stream.close();
			}

		private:
			std::ofstream stream;
	};
}