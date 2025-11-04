#pragma once

#include "AnnotationBlock.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ProjectA {
	class AST {
		public:
			uint32 getStatementCount() const {
				return statementCount;
			}

			uint32 getStatementBufferByteSize() const {
				return static_cast<uint32>(statementBuffer.size());
			}

			StatementType getStatementType(uint32 byteOffset) const {
				DEBUG_ASSERT(byteOffset + sizeof(StatementType) <= statementBuffer.size());

				return *pointer_cast<const StatementType*>(&statementBuffer[byteOffset]);
			}

			template<StatementType StatementType>
			const Statement<StatementType>& getStatement(uint32 byteOffset) const {
				DEBUG_ASSERT(byteOffset + sizeof(Statement<StatementType>) <= statementBuffer.size());

				return *pointer_cast<const Statement<StatementType>*>(&statementBuffer[byteOffset]);
			}

			template<StatementType StatementType>
			void addStatement(const Statement<StatementType>& statement) {
				const uint64 byteOffset = statementBuffer.size();

				statementBuffer.resize(byteOffset + sizeof(Statement<StatementType>));
				statementCount += 1;

				std::copy_n(pointer_cast<const uint8*>(&statement), sizeof(Statement<StatementType>), &statementBuffer[byteOffset]);
			}

			template<StatementType StatementType>
			void insertStatement(const Statement<StatementType>& statement, uint32 byteOffset) {
				DEBUG_ASSERT(byteOffset <= statementBuffer.size());

				std::vector<uint8> statementBytes(sizeof(Statement<StatementType>));

				std::copy_n(pointer_cast<const uint8*>(&statement), statementBytes.size(), statementBytes.begin());

				statementBuffer.insert(statementBuffer.begin() + byteOffset, statementBytes.begin(), statementBytes.end());
				statementCount += 1;
			}

			uint32 getExpressionCount() const {
				return expressionCount;
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
				expressionCount += 1;

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
				expressionCount += 1;

				std::copy_n(pointer_cast<const uint8*>(&expression), sizeof(Expression<ExpressionType>), &expressionBuffer[byteOffset]);

				ExpressionReference expressionReference{};
				expressionReference.type = ExpressionType;
				expressionReference.byteOffset = static_cast<uint32>(byteOffset);

				expressionReferences.insert(expressionReferences.begin() + index, expressionReference);
			}

			uint32 getAnnotationBlockCount() const {
				return static_cast<uint32>(annotationBlocks.size());
			}

			const AnnotationBlock& getAnnotationBlock(uint32 annotationBlockIndex) const {
				DEBUG_ASSERT(annotationBlockIndex < annotationBlocks.size());

				return annotationBlocks[annotationBlockIndex];
			}

			template<Annotation Annotation>
			const AnnotationBlock& getAnnotationBlock(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const {
				DEBUG_ASSERT(annotationBlockCount > 0);
				DEBUG_ASSERT(annotationBlockIndexOffset + annotationBlockCount < annotationBlocks.size());

				uint32 annotationBlockIndex = 0;

				while (annotationBlockIndex < annotationBlockCount - 1) {
					const AnnotationBlock& annotationBlock = annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];

					if (annotationBlock.annotation == Annotation) {
						return annotationBlock;
					}

					annotationBlockIndex += 1;
				}

				return annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];
			}

			void addAnnotationBlock(const AnnotationBlock& annotationBlock) {
				annotationBlocks.emplace_back(annotationBlock);
			}

		private:
			std::vector<uint8> statementBuffer;
			std::vector<uint8> expressionBuffer;
			std::vector<AnnotationBlock> annotationBlocks;

			// TODO: Remove.
			std::vector<ExpressionReference> expressionReferences;

			uint32 statementCount = 0;
			uint32 expressionCount = 0;
	};
}