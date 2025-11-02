#pragma once

#include "Language/Type.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct PackedType {
		static PackedType userDefined(uint32 typeIndex, Type templateArgument) {
			PackedType packedType{};
			packedType.typeIndex = static_cast<Type>(Type::USER_DEFINED + typeIndex);
			packedType.templateArgumentIndex = templateArgument;

			return packedType;
		}

		PackedType() = default;

		explicit PackedType(Type typeIndex, Type templateArgumentIndex) :
			typeIndex(typeIndex),
			templateArgumentIndex(templateArgumentIndex)
		{}

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
		Type templateArgumentIndex = Type::NONE;
	};
}