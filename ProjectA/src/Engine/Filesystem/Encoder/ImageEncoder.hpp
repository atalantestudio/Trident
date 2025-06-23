#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	namespace Filesystem {
		struct ParsedImage {
			// TODO
		};

		template<>
		ParsedImage decode<FileFormat::PNG>(const sequence<uint8>& data) {
			// TODO
		}
	}
}