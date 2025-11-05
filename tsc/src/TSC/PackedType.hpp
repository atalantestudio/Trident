#pragma once

#include "Language/Type.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		// NOTE: This method assumes that the template argument is not user-defined.
		static PackedType userDefined(uint32 typeIndex, uint32 templateArgumentIndex) {
			return {Type::USER_DEFINED + typeIndex, templateArgumentIndex};
		}

		bool hasUserDefinedType() const {
			return typeIndex >= Type::USER_DEFINED;
		}

		bool hasUserDefinedTemplateArgument() const {
			return templateArgumentIndex >= Type::USER_DEFINED;
		}

		bool operator ==(const PackedType& other) const {
			return typeIndex == other.typeIndex && templateArgumentIndex == other.templateArgumentIndex;
		}

		bool operator !=(const PackedType& other) const {
			return !(*this == other);
		}

		uint32 typeIndex = Type::NONE;
		uint32 templateArgumentIndex = Type::NONE;
	};
}