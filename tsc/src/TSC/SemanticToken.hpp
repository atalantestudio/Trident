#pragma once

#include "DocumentRange.hpp"
#include "SemanticTokenType.hpp"

namespace ProjectA {
	struct SemanticToken {
		uint32 getTypeIndex() const {
			return static_cast<uint32>(type);
		}

		DocumentRange range;
		SemanticTokenType type;
	};
}