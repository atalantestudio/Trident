#pragma once

namespace Trident {
	struct BaseFile {
		// File name, including extension.
		std::string name;

		// File absolute path.
		std::string absolutePath;

		// File size, in bytes.
		uint64 size;

		// Compressed file data.
		sequence<uint8> rawData;
	};
}