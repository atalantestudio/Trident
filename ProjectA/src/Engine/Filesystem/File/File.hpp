#pragma once

namespace ProjectA {
	struct File {
		// File name, including extension.
		std::string name;

		// File absolute path.
		std::string absolutePath;

		// File size, in bytes.
		uint64 size = 0;

		chain<uint8> rawData;
	};
}