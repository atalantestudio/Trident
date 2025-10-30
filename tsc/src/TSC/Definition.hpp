#pragma once

#include "DocumentPosition.hpp"
#include "DocumentRange.hpp"
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

		sint32 getEvaluatedInitializer() const {
			return static_cast<sint32>(evaluatedInitializer);
		}

		DocumentRange identifierRange;
		DocumentPosition end;
		PackedType type;
		SemanticTokenType semanticTokenType;
		uint32 tokenIndex;
		std::string identifier;

		// TODO: Adapt for constructors, e.g. `vec3<float32>(x, y, z)`.
		sint64 evaluatedInitializer;

		// NOTE (Structure definitions only)
		// Number of members in the structure.
		// The N member definitions directly follow the structure definition.
		uint32 memberCount;
	};
}