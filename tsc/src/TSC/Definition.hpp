#pragma once

#include "DocumentPosition.hpp"
#include "PackedType.hpp"
#include "SemanticTokenType.hpp"
#include "SemanticTokenModifier.hpp"

namespace ProjectA {
	struct Definition {
		uint32 getSemanticTokenTypeIndex() const {
			return static_cast<uint32>(semanticTokenType);
		}

		DocumentPosition end;
		PackedType type;
		SemanticTokenType semanticTokenType;
		uint32 semanticTokenModifierFlag;
		uint32 tokenIndex;
		std::string identifier;

		// TODO
		// uint32 parentStructureIndex = static_cast<uint32>(~0);
	};
}