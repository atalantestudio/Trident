#pragma once

#include "DocumentPosition.hpp"
#include "PackedType.hpp"
#include "SemanticTokenType.hpp"

namespace ProjectA {
	struct Definition {
		uint32 getTypeIndex() const {
			return type.typeIndex;
		}

		uint32 getTemplateArgumentIndex() const {
			return type.templateArgumentIndex;
		}

		uint32 getSemanticTokenTypeIndex() const {
			return static_cast<uint32>(semanticTokenType);
		}

		DocumentPosition identifierStart;
		DocumentPosition identifierEnd;
		DocumentPosition end;
		PackedType type;
		SemanticTokenType semanticTokenType;
		uint32 semanticTokenModifierFlag;
		uint32 tokenIndex;
		std::string identifier;

		// (Structure definitions only)
		// Number of members in the structure.
		// The N member definitions directly follow the structure definition.
		uint32 memberCount;
	};
}