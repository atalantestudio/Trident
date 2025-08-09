#pragma once

namespace ProjectA {
	class Reader {
		public:
			explicit Reader(std::string_view path) :
				stream(path.data())
			{}

			Reader& operator =(Reader& reader) {
				stream.swap(reader.getStream());

				return *this;
			}

			~Reader() {
				stream.close();
			}

			std::ifstream& getStream() {
				return stream;
			}

			bool fileExists() const {
				return stream.good();
			}

			void seekg(std::ios_base::seekdir way) {
				stream.seekg(0, way);
			}

			std::streampos tellg() {
				return stream.tellg();
			}

			sequence<uint8> read(uint64 count) {
				sequence<uint8> data(count);

				stream.read(pointer_cast<char8*>(data), data.count);

				return data;
			}

		private:
			std::ifstream stream;
	};
}