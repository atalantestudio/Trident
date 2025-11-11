#pragma once

namespace ProjectA {
	class Writer {
		public:
			explicit Writer(const str8& path) :
				stream(&path[0], std::ios::binary | std::ios::trunc)
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