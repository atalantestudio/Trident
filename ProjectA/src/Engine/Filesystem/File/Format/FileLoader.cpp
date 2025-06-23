#include "pch.hpp"

#include "Engine/Filesystem/File/BaseFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	BaseFile Filesystem::openTemporary(std::string_view path) {
		std::ifstream stream(path.data(), std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Could not open file \"[]\".", path);

		const std::filesystem::path absolutePath = std::filesystem::absolute(path);

		BaseFile file;
		file.name = absolutePath.filename().string();
		file.absolutePath = absolutePath.string();

		stream.seekg(0, std::ios::end);

		file.size = stream.tellg();

		stream.seekg(0, std::ios::beg);

		file.rawData = sequence<uint8>(file.size);

		stream.read(static_cast<char*>(static_cast<void*>(file.rawData)), file.size);
		stream.close();

		return file;
	}
}