#pragma once

#include "DocumentPosition.hpp"
#include "SemanticTokenType.hpp"

namespace ProjectA {
	class SemanticToken {
		public:
			uint32 getTypeIndex() const {
				return static_cast<uint32>(type);
			}

		public:
			DocumentPosition start;
			uint32 characterCount;
			SemanticTokenType type;
	};
}