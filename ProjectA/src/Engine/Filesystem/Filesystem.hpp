#pragma once

namespace ProjectA {
	class Filesystem {
		public:
			static std::filesystem::path getCurrentPath();

			static std::filesystem::path getCurrentDirectory();

			// The provided path is relative to the working directory.
			static chain<uint8> loadByteBuffer(const std::string& path);

			// The provided path is relative to the working directory.
			template<typename T>
			static T load(std::string_view path);
	};
}