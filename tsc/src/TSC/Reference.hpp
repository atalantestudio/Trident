#pragma once

#include "DocumentRange.hpp"

namespace ProjectA {
	struct Reference {
		// This index is relative to the scope's definition range.
		uint32 definitionIndex;

		DocumentRange identifierRange;
	};
}