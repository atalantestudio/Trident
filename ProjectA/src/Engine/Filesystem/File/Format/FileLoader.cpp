#include "pch.hpp"

#include "Engine/Filesystem/File/File.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	File Filesystem::load(const std::string& path) {
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
}