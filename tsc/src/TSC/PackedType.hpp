#pragma once

#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		PackedType() = default;

		explicit PackedType(uint32 typeIndex, uint32 templateArgumentIndex) {
			if (typeIndex >= TokenType::USER_DEFINED) {
				this->typeIndex = static_cast<uint16>(typeIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4));
			}
			else {
				this->typeIndex = static_cast<uint16>(typeIndex - TokenType::TYPE_FLOAT32);
			}

			if (templateArgumentIndex != 0) {
				if (templateArgumentIndex >= TokenType::USER_DEFINED) {
					this->templateArgumentIndex = static_cast<uint16>(templateArgumentIndex - (TokenType::USER_DEFINED - TokenType::TYPE_MAT4X4) + 1);
				}
				else {
					this->templateArgumentIndex = static_cast<uint16>(templateArgumentIndex - TokenType::TYPE_FLOAT32 + 1);
				}
			}
		}

		bool hasUserDefinedType() const {
			return typeIndex + TokenType::TYPE_FLOAT32 > TokenType::TYPE_MAT4X4;
		}

		bool hasUserDefinedTemplateArgument() const {
			return templateArgumentIndex + TokenType::TYPE_FLOAT32 - 1 > TokenType::TYPE_MAT4X4;
		}

		// This word represents the token type index, offsetted by -TokenType::TYPE_FLOAT32.
		// If it is greater than TokenType::TYPE_MAT4X4, it is a user-defined type. The type index is then offsetted by TokenType::USER_DEFINED.
		uint16 typeIndex;

		// This word is the token template argument index.
		// It uses the same rules as typeIndex but has an additional offset of -1, allowing it to represent a 'null' template argument.
		uint16 templateArgumentIndex = 0;
	};
}