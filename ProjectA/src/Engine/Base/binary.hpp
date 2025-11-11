#pragma once

#define STRING_TO_BIG_ENDIAN_UINT32(type) ( \
	(static_cast<uint32>(type[0]) << 24) | \
	(static_cast<uint32>(type[1]) << 16) | \
	(static_cast<uint32>(type[2]) << 8) | \
	(static_cast<uint32>(type[3]) << 0) \
)

#define STRING_TO_LITTLE_ENDIAN_UINT32(type) ( \
	(static_cast<uint32>(type[0]) << 0) | \
	(static_cast<uint32>(type[1]) << 8) | \
	(static_cast<uint32>(type[2]) << 16) | \
	(static_cast<uint32>(type[3]) << 24) \
)