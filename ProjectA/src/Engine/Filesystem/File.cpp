#include "pch.hpp"

#include "Engine/Filesystem/File.hpp"

namespace ProjectA {
	void File::swapEndianness(uint16* value) {
		*value = _byteswap_ushort(*value);
	}

	void File::swapEndianness(uint32* value) {
		*value = _byteswap_ulong(*value);
	}

	File::File(const std::string& path)
		: stream(path, std::ios::binary)
	{
		if (!stream.is_open()) {
			throw std::runtime_error("Failed to open file.");
		}
	}

	File::~File() {
		stream.close();
	}

	std::streampos File::getPosition() {
		return stream.tellg();
	}

	bool File::eof() {
		return stream.peek() == EOF;
	}

	void File::advance(std::streamsize byteCount) {
		stream.ignore(byteCount);
	}
}	