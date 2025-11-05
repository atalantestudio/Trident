#pragma once

#include "PackedType.hpp"

namespace ProjectA {
	class ExpressionEvaluation {
		public:
			ExpressionEvaluation() = default;

			explicit ExpressionEvaluation(PackedType type) :
				type(type),
				intermediateType(type)
			{}

			sint64 getValue() const {
				return components[lastComponentIndex];
			}

			void setValue(sint64 value) {
				components[lastComponentIndex] = value;
			}

			void addComponent(sint64 value) {
				components.emplace_back(value);
				lastComponentIndex += 1;
			}

		public:
			// The type the expression must evaluate to in the end.
			PackedType type;

			// An intermediate type the expression is explicitly allowed to evaluate to while being analyzed.
			PackedType intermediateType;

			bool casting = false;

			// The expressions is marked as constant by default, but this can change while evaluating.
			bool constant = true;

		private:
			std::vector<sint64> components = {0};
			uint32 lastComponentIndex = 0;
	};
}