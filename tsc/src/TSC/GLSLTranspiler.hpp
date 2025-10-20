#pragma once

// TODO: Remove.
#include "pch.hpp"

#include "AST.hpp"
#include "CompilationResult.hpp"

namespace ProjectA {
	class GLSLTranspiler {
		public:
			void write(const char8* path, const CompilationResult& result, const AST& ast);

		private:
			void writeStatement(uint32 globalScopeStatementIndex);
			void writeStructureStatement(const ref<StructureStatement> statement);
			void writeMemberStatement(const ref<VariableStatement> memberStatement);

			void writeTypeExpression(const ref<TypeExpression> expression);

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

						return "_" + result->identifiers[typeToken.type - TokenType::USER_DEFINED];
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