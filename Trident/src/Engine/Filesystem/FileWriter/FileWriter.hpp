// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/FileFormat.hpp"

namespace Trident {
	template<FileFormat FileFormat>
	class FileWriter;

	class BaseFileWriter {
		public:
			explicit BaseFileWriter(view<char8> path) : 
				stream(&path[0], std::ios::out | std::ios::trunc)
			{
				ENGINE_ASSERT(stream.is_open(), "Cannot open \"[]\" for file logging.", path);
			}

			~BaseFileWriter() {
				stream.close();
			}

		protected:
			std::ofstream stream;
	};
}