#pragma once

namespace ProjectA {
	class Reader {
		public:
			Reader(std::string_view path) :
				stream(path.data(), std::ios::binary)
			{
				ENGINE_ASSERT(stream.is_open(), "File not found.");
			}

			~Reader() {
				stream.close();
			}

			void read(const sequence<uint8>& data) {
				// TODO
				// stream.read(static_cast<const char*>(static_cast<const void*>(data)), data.count);
			}

		private:
			std::ifstream stream;
	};
}