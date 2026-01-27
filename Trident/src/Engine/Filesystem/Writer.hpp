// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Writer {
		public:
			explicit Writer(view<char8> path) :
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