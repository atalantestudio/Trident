#pragma once

namespace ProjectA {
	struct File {
		// File name, including extension.
		std::string name;

		// File absolute path.
		std::string absolutePath;

		// File size, in bytes.
		uint64 size = 0;

		// TODO: Remove this member. No need to keep this data when we have also the decompressed data.
		// This member should be file-specific.
		// Compressed file data.
		std::vector<uint8> rawData;
	};
}