#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	void Filesystem::initialize() {
		const std::filesystem::path currentDirectoryName = std::filesystem::current_path().filename();

		if (currentDirectoryName != "Sandbox" && currentDirectoryName != "Editor") {
			if (currentDirectoryName != "Sandbox") {
				std::filesystem::current_path("../../../Sandbox");
			}
			else if (currentDirectoryName != "Editor") {
				std::filesystem::current_path("../../../Editor");
			}
		}
	}

	chain<uint8> Filesystem::loadByteBuffer(const std::string& path) {
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
}