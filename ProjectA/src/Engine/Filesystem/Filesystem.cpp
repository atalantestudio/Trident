#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	std::filesystem::path Filesystem::getProjectRootDirectory() {
		return std::filesystem::current_path();
	}

	std::filesystem::path Filesystem::getCurrentDirectory() {
		return getProjectRootDirectory().filename();
	}

	sequence<uint8> Filesystem::openByteBuffer(view<char8> path) {
		std::ifstream stream(&path[0], std::ios::ate | std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		const std::streampos count = stream.tellg();

		stream.seekg(0, std::ios::beg);

		sequence<uint8> buffer(count);

		if (count > 0) {
			stream.read(pointer_cast<char8*>(&buffer[0]), count);
		}

		stream.close();

		return buffer;
	}

	sequence<char8> Filesystem::openTextBuffer(view<char8> path) {
		// NOTE
		// std::ios::binary was added because of hangs in the shader validator when creating SPIR-V shaders in either OpenGL or Vulkan.
		// See also https://stackoverflow.com/a/26993247.
		std::ifstream stream(&path[0], std::ios::ate | std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		const std::streampos count = stream.tellg();

		stream.seekg(0, std::ios::beg);

		sequence<char8> buffer(count);

		if (count > 0) {
			stream.read(&buffer[0], count);
		}

		stream.close();

		return buffer;
	}

	Reader Filesystem::openReader(view<char8> path) {
		return Reader(path);
	}

	Writer Filesystem::openWriter(view<char8> path) {
		return Writer(path);
	}
}