#pragma once

#include "AnnotationBlock.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ProjectA {
	class AST {
		public:
			uint32 getExpressionReferenceCount() const {
				return static_cast<uint32>(expressionReferences.size());
			}

			const ExpressionReference& getExpressionReference(uint32 index) const {
				DEBUG_ASSERT(index < expressionReferences.size());

				return expressionReferences[index];
			}

			template<ExpressionType ExpressionType>
			const Expression<ExpressionType>& getExpression(uint32 byteOffset) const {
				DEBUG_ASSERT(byteOffset + sizeof(Expression<ExpressionType>) <= expressionBuffer.size());

				return *pointer_cast<const Expression<ExpressionType>*>(&expressionBuffer[byteOffset]);
			}

			template<ExpressionType ExpressionType>
			void addExpression(const Expression<ExpressionType>& expression) {
				const uint64 byteOffset = expressionBuffer.size();

				expressionBuffer.resize(byteOffset + sizeof(Expression<ExpressionType>));

				std::copy_n(pointer_cast<const uint8*>(&expression), sizeof(Expression<ExpressionType>), &expressionBuffer[byteOffset]);

				ExpressionReference expressionReference{};
				expressionReference.type = ExpressionType;
				expressionReference.byteOffset = static_cast<uint32>(byteOffset);

				expressionReferences.emplace_back(expressionReference);
			}

			template<ExpressionType ExpressionType>
			void insertExpression(const Expression<ExpressionType>& expression, uint32 index) {
				const uint64 byteOffset = expressionBuffer.size();

				expressionBuffer.resize(byteOffset + sizeof(Expression<ExpressionType>));

				std::copy_n(pointer_cast<const uint8*>(&expression), sizeof(Expression<ExpressionType>), &expressionBuffer[byteOffset]);

				ExpressionReference expressionReference{};
				expressionReference.type = ExpressionType;
				expressionReference.byteOffset = static_cast<uint32>(byteOffset);

				expressionReferences.insert(expressionReferences.begin() + index, expressionReference);
			}

		public:
			std::vector<ref<Statement>> statements;
			std::vector<uint32> globalScopeStatementIndices;

			std::vector<ExpressionReference> expressionReferences;
			std::vector<uint8> expressionBuffer;

			std::vector<AnnotationBlock> annotationBlocks;
	};
}