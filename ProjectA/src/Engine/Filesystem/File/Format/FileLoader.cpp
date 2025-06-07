#include "pch.hpp"

#include "Engine/Filesystem/File/File.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	File Filesystem::openTemporary(std::string_view path) {
		std::ifstream stream(path.data(), std::ios::binary);

		ENGINE_ASSERT(stream.is_open(), "Could not open file \"[]\".", path);

		const std::filesystem::path absolutePath = std::filesystem::absolute(path);

		File file;
		file.name = absolutePath.filename().string();
		file.absolutePath = absolutePath.string();

		stream.seekg(0, std::ios::end);

		file.size = stream.tellg();

		stream.seekg(0, std::ios::beg);

		file.rawData.resize(file.size);

		stream.read(static_cast<char*>(static_cast<void*>(file.rawData.data())), file.size);
		stream.close();

		return file;
	}
}