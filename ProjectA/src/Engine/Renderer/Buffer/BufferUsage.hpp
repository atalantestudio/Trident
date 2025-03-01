#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class BufferUsage : uint32 {
		UNIFORM = 0,
		STORAGE = 1,
		INDEX = 2,
		VERTEX = 3,
		INDIRECT = 4,
	};
}