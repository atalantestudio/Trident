#pragma once

#include "Engine/Filesystem/FileFormat.hpp"

namespace ProjectA {
	template<FileFormat FileFormat>
	class FileWriter;

	class BaseFileWriter {
		public:
			explicit BaseFileWriter(const str8& path) : 
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