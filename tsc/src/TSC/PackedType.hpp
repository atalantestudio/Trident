#pragma once

#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		PackedType() = default;

		explicit PackedType(uint32 typeIndex, uint32 templateArgumentIndex) {
			this->typeIndex = static_cast<uint16>(typeIndex - TokenType::TYPE_FLOAT32);
			this->templateArgumentIndex = static_cast<uint16>(templateArgumentIndex + 1 - TokenType::TYPE_FLOAT32);

			if (this->typeIndex > TokenType::USER_DEFINED) {
				this->typeIndex -= TokenType::USER_DEFINED;
			}

			if (this->templateArgumentIndex > TokenType::USER_DEFINED) {
				this->templateArgumentIndex -= TokenType::USER_DEFINED;
			}
		}

		// This word represents the token type index, offsetted by -TokenType::TYPE_FLOAT32.
		// If it is greater than TokenType::TYPE_MAT4X4, it is a user-defined type. The type index is then offsetted by TokenType::TYPE_MAT4X4.
		uint16 typeIndex;

		// This word is the token template argument index.
		// It uses the same rules as typeIndex but has an additional offset of +1, allowing it to represent a 'null' template argument.
		uint16 templateArgumentIndex;
	};
}