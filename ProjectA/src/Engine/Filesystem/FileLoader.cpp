#include "pch.hpp"

#include "Engine/Filesystem/FileLoader.hpp"

namespace ProjectA {
	std::string FileLoader::load(const std::string& path) {
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file.");
		}

		const std::string buffer(std::istreambuf_iterator<char>(file), {});

		file.close();

		return buffer;
	}

	/*chain<uint8> FileLoader::loadBuffer(const std::string& path) {
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file.");
		}

		file.seekg(0, std::ios::end);

		const std::streampos size = file.tellg();

		file.seekg(0, std::ios::beg);

		chain<uint8> buffer(size);

		file.read(static_cast<char*>(static_cast<void*>(buffer.data())), size);
		file.close();

		return buffer;
	}*/
}