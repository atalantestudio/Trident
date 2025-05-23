#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	std::filesystem::path Filesystem::getCurrentPath() {
		return std::filesystem::current_path();
	}

	std::filesystem::path Filesystem::getCurrentDirectory() {
		return getCurrentPath().filename();
	}

	std::vector<uint8> Filesystem::loadByteBuffer(const std::string& path) {
		std::ifstream stream(path, std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		stream.seekg(0, std::ios::end);

		const std::streampos size = stream.tellg();

		stream.seekg(0, std::ios::beg);

		std::vector<uint8> buffer(size);

		stream.read(static_cast<char*>(static_cast<void*>(buffer.data())), size);
		stream.close();

		return buffer;
	}
}