#pragma once

namespace ProjectA {
	class Writer {
		public:
			explicit Writer(std::string_view path) :
				stream(path.data())
			{
				ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);
			}

			~Writer() {
				stream.close();
			}

			void write(const char* string, uint64 size) {
				stream.write(string, size);
			}

			void write(const sequence<uint8>& data) {
				stream.write(pointer_cast<const char*>(data), data.count);
			}

		private:
			std::ofstream stream;
	};
}