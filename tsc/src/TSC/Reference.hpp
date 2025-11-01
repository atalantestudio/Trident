#pragma once

#include "DocumentRange.hpp"

namespace ProjectA {
	struct Reference {
		uint32 definitionIndex;
		uint32 identifierIndex;
		DocumentRange identifierRange;
	};
}