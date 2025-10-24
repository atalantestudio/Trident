#pragma once

#include "Engine/Filesystem/FileFormat.hpp"

namespace ProjectA {
	template<FileFormat FileFormat>
	class FileLogger;

	class BaseFileLogger {
		public:
			explicit BaseFileLogger(std::string_view path) : 
				stream(path.data(), std::ios::out | std::ios::trunc)
			{
				ENGINE_ASSERT(stream.is_open(), "Cannot open \"[]\" for file logging.", path);
			}

			~BaseFileLogger() {
				stream.close();
			}

		protected:
			std::ofstream stream;
	};
}