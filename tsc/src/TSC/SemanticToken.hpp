#pragma once

#include "Position.hpp"
#include "SemanticTokenModifier.hpp"
#include "SemanticTokenType.hpp"

namespace ProjectA {
	// TODO: Find a way to serialize without having to write getters.
	class SemanticToken {
		public:
			uint32 getTypeIndex() const {
				return static_cast<uint32>(type);
			}

			uint32 getModifiers() const {
				return modifiers;
			}

			uint32 getLineIndex() const {
				return start.lineIndex;
			}

			uint32 getCharacterOffset() const {
				return start.characterOffset;
			}

			uint32 getCharacterCount() const {
				return characterCount;
			}

		public:
			SemanticTokenType type;
			uint32 modifiers;
			Position start;
			uint32 characterCount;
	};
}