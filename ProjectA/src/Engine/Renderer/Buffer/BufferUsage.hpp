#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class BufferUsage : uint16 {
		TRANSFER_SRC = BIT(0),
		TRANSFER_DST = BIT(1),
		UNIFORM = BIT(4),
		STORAGE = BIT(5),
		INDEX = BIT(6),
		VERTEX = BIT(7),
		INDIRECT = BIT(8),
	};

	AS_FLAG(BufferUsage);
}