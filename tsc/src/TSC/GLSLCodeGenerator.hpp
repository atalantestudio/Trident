#pragma once

#include "AST.hpp"
#include "CompilationResult.hpp"

namespace ProjectA {
	class GLSLCodeGenerator {
		public:
			void write(const char8* path, const CompilationResult& result, const AST& ast);

		private:
			void writeStatement(uint32 statementIndex);
			void writeGlobalScopeConstant(uint32 statementIndex);
			void writeStructureStatement(uint32 statementIndex);
			void writeMemberStatement(uint32 statementIndex);
			void writeFunctionStatement(uint32 statementIndex);
			void writeVariableStatement(uint32 statementIndex);
			void writeParameterStatement(uint32 statementIndex);
			void writeScopeStatement(uint32 statementIndex);
			void writeIfStatement(uint32 statementIndex);
			void writeWhileStatement(uint32 statementIndex);
			void writeBreakStatement();
			void writeNextStatement();
			void writeReturnStatement(uint32 statementIndex);
			void writeExpressionStatement(uint32 statementIndex);

			void writeExpression(uint32 expressionIndex);
			void writeNumberExpression(uint32 expressionIndex);
			void writeIdentifierExpression(uint32 expressionIndex);
			void writeUnaryExpression(uint32 expressionIndex);
			void writeBinaryExpression(uint32 expressionIndex);
			void writeMemberAccessExpression(uint32 expressionIndex);
			void writeTypeExpression(uint32 expressionIndex);

			void writeAnnotations(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount);

			std::string indent() const {
				return std::string(_indent, '\t');
			}

			std::string getGlslTemplateArgumentPrefix(const Token& templateArgumentToken) const {
				switch (templateArgumentToken.type) {
					case TokenType::TYPE_FLOAT32:
						return "";
					case TokenType::TYPE_FLOAT64:
						return "d";
					case TokenType::TYPE_SINT32:
						return "i";
					case TokenType::TYPE_UINT32:
						return "u";
					default: {
						ENGINE_ASSERT(false, "Invalid type.");

						return "";
					}
				}
			}

			std::string getGlslType(uint32 typeTokenIndex, uint32 templateArgumentTokenIndex) const {
				const Token& typeToken = result->tokens[typeTokenIndex];

				switch (typeToken.type) {
					case TokenType::TYPE_FLOAT32:
						return "float";
					case TokenType::TYPE_FLOAT64:
						return "double";
					case TokenType::TYPE_SINT32:
						return "int";
					case TokenType::TYPE_UINT32:
						return "uint";
					case TokenType::TYPE_VEC2:
					case TokenType::TYPE_VEC3:
					case TokenType::TYPE_VEC4:
					case TokenType::TYPE_MAT2X2:
					case TokenType::TYPE_MAT2X3:
					case TokenType::TYPE_MAT2X4:
					case TokenType::TYPE_MAT3X2:
					case TokenType::TYPE_MAT3X3:
					case TokenType::TYPE_MAT3X4:
					case TokenType::TYPE_MAT4X2:
					case TokenType::TYPE_MAT4X3:
					case TokenType::TYPE_MAT4X4: {
						const std::string typeString = getTokenString(typeToken);

						if (templateArgumentTokenIndex == ~0) {
							return typeString;
						}

						const Token& templateArgumentToken = result->tokens[templateArgumentTokenIndex];

						return getGlslTemplateArgumentPrefix(templateArgumentToken) + typeString;
					}
					default: {
						ENGINE_ASSERT(typeToken.type >= TokenType::USER_DEFINED, "Invalid type.");

						return result->identifiers[typeToken.type - TokenType::USER_DEFINED];
					}
				}
			}

		private:
			std::ofstream stream;
			uint32 _indent = 0;
			const CompilationResult* result;
			const AST* ast;
	};
}