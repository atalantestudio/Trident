#pragma once

#include "DocumentPosition.hpp"

namespace ProjectA {
	static constexpr uint32 UNDEFINED_SCOPE = ~0;

	struct Scope {
		DocumentPosition start;
		DocumentPosition end;
		uint32 parentScopeIndex;
		uint32 definitionIndexOffset;
		uint32 definitionCount;
	};
}