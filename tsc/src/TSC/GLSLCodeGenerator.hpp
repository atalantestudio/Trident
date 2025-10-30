#pragma once

#include "TSC/CompilationOutput.hpp"
#include "TSC/Token.hpp"
#include "TSC/TokenType.hpp"

namespace ProjectA {
	class GLSLCodeGenerator {
		public:
			static constexpr const char8* GLSL_TYPE_PREFIXES[] = {
				"", // float
				"d", // double
				"i", // int
				"u", // uint
			};

		public:
			void write(std::ofstream& stream, const CompilationOutput& output);

		private:
			void writeStatement(std::ofstream& stream, uint32 statementIndex);
			void writeStructureStatement(std::ofstream& stream, uint32 statementIndex);
			void writeMemberStatement(std::ofstream& stream, uint32 statementIndex);
			void writeConstantStatement(std::ofstream& stream, uint32 statementIndex);
			void writeVariableStatement(std::ofstream& stream, uint32 statementIndex);
			void writeParameterStatement(std::ofstream& stream, uint32 statementIndex);
			void writeFunctionStatement(std::ofstream& stream, uint32 statementIndex);
			void writeExpressionStatement(std::ofstream& stream, uint32 statementIndex);
			void writeScopeStatement(std::ofstream& stream, uint32 statementIndex);
			void writeIfStatement(std::ofstream& stream, uint32 statementIndex);
			void writeWhileStatement(std::ofstream& stream, uint32 statementIndex);
			void writeBreakStatement(std::ofstream& stream);
			void writeNextStatement(std::ofstream& stream);
			void writeReturnStatement(std::ofstream& stream, uint32 statementIndex);

			void writeNextExpression(std::ofstream& stream);
			void writeLiteralExpression(std::ofstream& stream, uint32 byteOffset);
			void writeIdentifierExpression(std::ofstream& stream, uint32 byteOffset);
			void writeUnaryExpression(std::ofstream& stream, uint32 byteOffset);
			void writeBinaryExpression(std::ofstream& stream, uint32 byteOffset);
			void writeGroupExpression(std::ofstream& stream, uint32 byteOffset);
			void writeMemberAccessExpression(std::ofstream& stream, uint32 byteOffset);
			void writeCallExpression(std::ofstream& stream, uint32 byteOffset);
			void writeTypeExpression(std::ofstream& stream, uint32 byteOffset);

			void writeAnnotationBlocks(std::ofstream& stream, uint32 indexOffset, uint32 count);

			void indent();
			void unindent();

			std::string getGlslType(uint32 typeTokenIndex, uint32 templateArgumentTokenIndex) const {
				const Token& typeToken = output->tokens[typeTokenIndex];

				switch (typeToken.type) {
					case TokenType::TYPE_FLOAT32: {
						return "float";
					}
					case TokenType::TYPE_FLOAT64: {
						return "double";
					}
					case TokenType::TYPE_SINT32: {
						return "int";
					}
					case TokenType::TYPE_UINT32: {
						return "uint";
					}
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
						const std::string typeString = output->getTokenString(typeToken);

						if (templateArgumentTokenIndex == UNDEFINED_TOKEN) {
							return typeString;
						}

						const Token& templateArgumentToken = output->tokens[templateArgumentTokenIndex];

						return GLSL_TYPE_PREFIXES[templateArgumentToken.type - TokenType::TYPE_FLOAT32] + typeString;
					}
					default: {
						MODULE_ASSERT(typeToken.type == TokenType::IDENTIFIER, "Invalid type.");

						return output->identifiers[typeToken.index];
					}
				}
			}

		private:
			const CompilationOutput* output;

			uint32 currentExpressionReferenceIndex;
			uint32 indentationLevel;
			std::string indentation;
	};
}