#pragma once

#include "Language/BaseType.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		static PackedType fromTypeExpression(TokenType type, uint32 typeIndex, uint32 templateArgumentIndex) {
			PackedType packedType;

			if (type >= TokenType::TYPE_USER_DEFINED) {
				packedType.typeIndex = static_cast<BaseType>(BaseType::USER_DEFINED + typeIndex);
			}
			else {
				packedType.typeIndex = static_cast<BaseType>(type - TokenType::TYPE_FLOAT32 + 1);
			}

			if (templateArgumentIndex > 0) {
				packedType.templateArgumentIndex = static_cast<BaseType>(templateArgumentIndex - TokenType::TYPE_FLOAT32 + 1);
			}

			return packedType;
		}

		PackedType() = default;

		explicit PackedType(BaseType type, BaseType templateArgument) :
			typeIndex(type),
			templateArgumentIndex(templateArgument)
		{}

		bool hasUserDefinedType() const {
			return typeIndex > BaseType::USER_DEFINED;
		}

		bool hasUserDefinedTemplateArgument() const {
			return templateArgumentIndex > BaseType::USER_DEFINED;
		}

		bool operator ==(const PackedType& other) const {
			return typeIndex == other.typeIndex && templateArgumentIndex == other.templateArgumentIndex;
		}

		bool operator !=(const PackedType& other) const {
			return !operator ==(other);
		}

		BaseType typeIndex = BaseType::OTHER;
		BaseType templateArgumentIndex = BaseType::OTHER;
	};
}