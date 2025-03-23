#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	class FileLoader {
		public:
			static File loadFile(const std::string& path);
			static chain<uint8> loadByteBuffer(const std::string& path);
	};
}