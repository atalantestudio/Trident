#pragma once

#include "Language/BaseType.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		static PackedType fromTypeExpression(uint32 typeIndex, uint32 templateArgumentIndex) {
			PackedType packedType;

			if (typeIndex >= TokenType::USER_DEFINED) {
				packedType.typeIndex = static_cast<uint16>(typeIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4));
			}
			else {
				packedType.typeIndex = static_cast<uint16>(typeIndex - TokenType::TYPE_FLOAT32);
			}

			if (templateArgumentIndex != 0) {
				if (templateArgumentIndex >= TokenType::USER_DEFINED) {
					packedType.templateArgumentIndex = static_cast<uint16>(templateArgumentIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4) + 1);
				}
				else {
					packedType.templateArgumentIndex = static_cast<uint16>(templateArgumentIndex - TokenType::TYPE_FLOAT32 + 1);
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
			return typeIndex + TokenType::TYPE_FLOAT32 > TokenType::TYPE_MAT4X4;
		}

		bool hasUserDefinedTemplateArgument() const {
			return templateArgumentIndex + TokenType::TYPE_FLOAT32 - 1 > TokenType::TYPE_MAT4X4;
		}

		// This word represents the token type index, offsetted by -TokenType::TYPE_FLOAT32.
		// If it is greater than TokenType::TYPE_MAT4X4, it is a user-defined type. The type index is then offsetted by TokenType::USER_DEFINED.
		uint16 typeIndex = 0;

		// This word is the token template argument index.
		// It uses the same rules as typeIndex but has an additional offset of -1, allowing it to represent a 'null' template argument.
		uint16 templateArgumentIndex = 0;
	};
}