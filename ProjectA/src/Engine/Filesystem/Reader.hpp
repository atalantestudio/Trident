#pragma once

namespace ProjectA {
	class Reader {
		public:
			explicit Reader(view<char8> path) :
				stream(&path[0])
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

				stream.read(pointer_cast<char8*>(&data[0]), data.count());

				return data;
			}

		private:
			std::ifstream stream;
	};
}