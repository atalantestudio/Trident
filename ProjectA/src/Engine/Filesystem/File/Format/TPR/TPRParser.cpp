#include "pch.hpp"

#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	PropertyFile Filesystem::load(const std::string& path) {
		return PropertyFile(path);
	}
}