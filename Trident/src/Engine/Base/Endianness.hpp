#pragma once

namespace Trident {
	enum class Endianness : uint8 {
		BIG_ENDIAN = 0,
		LITTLE_ENDIAN = 1,
	};

	// Big-endian.
	static constexpr uint8 BE = static_cast<uint8>(Endianness::BIG_ENDIAN);

	// Little-endian.
	static constexpr uint8 LE = static_cast<uint8>(Endianness::LITTLE_ENDIAN);
}