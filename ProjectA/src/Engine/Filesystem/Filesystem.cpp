#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	std::filesystem::path Filesystem::getProjectRootDirectory() {
		return std::filesystem::current_path();
	}

	std::filesystem::path Filesystem::getCurrentDirectory() {
		return getProjectRootDirectory().filename();
	}

	sequence<uint8> Filesystem::openBuffer(std::string_view path) {
		std::ifstream stream(path.data(), std::ios::ate | std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		const std::streampos count = stream.tellg();

		stream.seekg(0, std::ios::beg);

		sequence<uint8> buffer(count);

		stream.read(pointer_cast<char8*>(&buffer[0]), count);
		stream.close();

		return buffer;
	}

	Reader Filesystem::openReader(std::string_view path) {
		return Reader(path);
	}

	Writer Filesystem::openWriter(std::string_view path) {
		return Writer(path);
	}
}