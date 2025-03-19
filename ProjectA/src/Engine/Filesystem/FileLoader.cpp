#include "pch.hpp"

#include "Engine/Filesystem/FileLoader.hpp"

namespace ProjectA {
	File FileLoader::loadFile(const std::string& path) {
		std::ifstream stream(path, std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		const std::filesystem::path absolutePath = std::filesystem::absolute(path);

		stream.seekg(0, std::ios::end);

		File file;
		file.name = absolutePath.filename().string();
		file.absolutePath = absolutePath.string();
		file.size = stream.tellg();
		file.rawData.resize(file.size);

		stream.seekg(0, std::ios::beg);
		stream.read(static_cast<char*>(static_cast<void*>(file.rawData.data())), file.size);
		stream.close();

		return file;
	}

	chain<uint8> FileLoader::loadByteBuffer(const std::string& path) {
		std::ifstream stream(path, std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		stream.seekg(0, std::ios::end);

		const std::streampos size = stream.tellg();

		stream.seekg(0, std::ios::beg);

		chain<uint8> chain(size);

		stream.read(static_cast<char*>(static_cast<void*>(chain.data())), size);
		stream.close();

		return chain;
	}

	std::string FileLoader::loadString(const std::string& path) {
		std::ifstream stream(path, std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Failed to open file \"[]\".", path);

		const std::string buffer(std::istreambuf_iterator<char>(stream), {});

		stream.close();

		return buffer;
	}
}