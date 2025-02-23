#include "pch.hpp"

#include "ProjectA/Loader/FileLoader.hpp"

namespace ProjectA {
	const std::string FileLoader::load(const std::string& path) {
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file.");
		}

		const std::string buffer(std::istreambuf_iterator<char>(file), {});

		file.close();

		return buffer;
	}
}