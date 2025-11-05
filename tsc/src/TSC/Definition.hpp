#pragma once

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

		// TODO: Store the full definition range.
		DocumentPosition end;

		DocumentRange identifierRange;
		PackedType type;
		SemanticTokenType semanticTokenType;
		uint32 identifierTokenIndex;
		uint32 identifierIndex;

		// TODO: Adapt for constructors, e.g. `vec3<float32>(x, y, z)`.
		sint64 evaluatedInitializer;

		// NOTE: Only used by the compiler to generate unused object info diagnostics.
		uint32 referenceCount = 0;

		// NOTE (Structure definitions only)
		// Number of members in the structure.
		// The N member definitions directly follow the structure definition.
		uint32 memberCount = 0;

		// NOTE (Member definitions only)
		// Index of the structure definition that contains this member.
		uint32 parentDefinitionIndex = static_cast<uint32>(~0);
	};
}