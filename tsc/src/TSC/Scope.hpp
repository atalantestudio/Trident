#pragma once

#include "DocumentPosition.hpp"

namespace ProjectA {
	struct Scope {
		DocumentRange range;
		uint32 parentScopeIndex;

		uint32 definitionIndexOffset;
		uint32 definitionCount;

		uint32 referenceIndexOffset;
		uint32 referenceCount;
	};
}