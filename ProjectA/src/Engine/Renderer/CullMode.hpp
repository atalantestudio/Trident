#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class CullMode : uint32 {
		NONE = 0,
		FRONT = 1,
		BACK = 2,
		FRONT_AND_BACK = 3,
	};
}