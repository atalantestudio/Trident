#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	TTFFile Filesystem::load(const std::string& path) {
		const File file = load<File>(path);

		TTFFile ttfFile;
		ttfFile.name = file.name;
		ttfFile.absolutePath = file.absolutePath;
		ttfFile.size = file.size;
		ttfFile.rawData = file.rawData;

		// TODO

		return ttfFile;
	}
}