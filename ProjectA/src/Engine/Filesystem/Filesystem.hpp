#pragma once

#include "Engine/Filesystem/File/ConfigurationFile.hpp"

namespace ProjectA {
	class Filesystem {
		public:
			static void initialize();

			// The provided path is relative to the working directory.
			static chain<uint8> loadByteBuffer(const std::string& path);

			// The provided path is relative to the working directory.
			template<typename T>
			static T load(const std::string& path);
	};
}