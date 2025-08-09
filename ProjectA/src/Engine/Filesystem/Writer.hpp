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

			void write(const uint8* data, uint64 size) {
				stream.write(pointer_cast<const char8*>(data), size);
			}

		private:
			std::ofstream stream;
	};
}