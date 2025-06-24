#include "pch.hpp"

#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	PropertyFile* Filesystem::open/*<OpenMode::AT_END>*/(std::string_view path) {
		std::ifstream stream(path.data());

		if (!stream.is_open()) {
			return nullptr;
		}

		return new PropertyFile(path);
	}
}