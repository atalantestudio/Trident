#pragma once

#include "Engine/Filesystem/OpenMode.hpp"

namespace ProjectA {
	class Filesystem {
		public:
			static std::filesystem::path getProjectRootDirectory();

			static std::filesystem::path getCurrentDirectory();

			// Returns the data of the file at the provided path as a byte sequence.
			// `path` is relative to the working directory.
			static sequence<uint8> openBuffer(std::string_view path);

			// Opens a file for temporary reading or writing.
			// `path` is relative to the working directory.
			template<typename File>
			static File openTemporary(std::string_view path);

			// Opens a file for permanent reading or writing.
			// `path` is relative to the working directory.
			// The returned value is the file pointer. It must be managed by the application.
			template<typename File/*, OpenMode OpenMode*/>
			static File* open(std::string_view path);
	};
}