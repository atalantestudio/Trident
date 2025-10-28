#pragma once

#include "Language/BaseType.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		static PackedType fromTypeExpression(uint32 typeIndex, uint32 templateArgumentIndex) {
			PackedType packedType;

			if (typeIndex >= TokenType::USER_DEFINED) {
				packedType.typeIndex = static_cast<BaseType>(typeIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4) + 1);
			}
			else {
				packedType.typeIndex = static_cast<BaseType>(typeIndex - TokenType::TYPE_FLOAT32 + 1);
			}

			if (templateArgumentIndex != 0) {
				if (templateArgumentIndex >= TokenType::USER_DEFINED) {
					packedType.templateArgumentIndex = static_cast<BaseType>(templateArgumentIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4) + 1);
				}
				else {
					packedType.templateArgumentIndex = static_cast<BaseType>(templateArgumentIndex - TokenType::TYPE_FLOAT32 + 1);
				}
			}

			return packedType;
		}

		PackedType() = default;

		explicit PackedType(BaseType type, BaseType templateArgument) :
			typeIndex(type),
			templateArgumentIndex(templateArgument)
		{}

		bool hasUserDefinedType() const {
			return typeIndex + TokenType::TYPE_FLOAT32 - 1 > TokenType::TYPE_MAT4X4;
		}

		bool hasUserDefinedTemplateArgument() const {
			return templateArgumentIndex + TokenType::TYPE_FLOAT32 - 1 > TokenType::TYPE_MAT4X4;
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