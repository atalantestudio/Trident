#pragma once

#include "DocumentRange.hpp"

namespace ProjectA {
	struct Reference {
		uint32 definitionIndex;
		DocumentRange identifierRange;
	};
}