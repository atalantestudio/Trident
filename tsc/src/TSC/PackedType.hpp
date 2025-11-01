#pragma once

#include "Language/Type.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		static PackedType fromTypeExpression(TokenType type, uint32 typeIdentifierIndex, uint32 templateArgumentIndex) {
			PackedType packedType;

			if (type == TokenType::IDENTIFIER) {
				packedType.typeIndex = Type::NONE;
				packedType.typeIdentifierIndex = typeIdentifierIndex;
			}
			else {
				packedType.typeIndex = static_cast<Type>(type - TokenType::TYPE_FLOAT32 + 1);
			}

			if (templateArgumentIndex > 0) {
				packedType.templateArgumentIndex = static_cast<Type>(templateArgumentIndex - TokenType::TYPE_FLOAT32 + 1);
			}

			return packedType;
		}

		PackedType() = default;

		explicit PackedType(Type typeIndex, Type templateArgumentIndex) :
			typeIndex(typeIndex),
			templateArgumentIndex(templateArgumentIndex)
		{}

		bool unfinished() const {
			return typeIndex == Type::NONE;
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
			return !operator ==(other);
		}

		Type typeIndex = Type::NONE;
		uint32 typeIdentifierIndex = 0;
		Type templateArgumentIndex = Type::NONE;
	};
}