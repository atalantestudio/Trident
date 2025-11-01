#include "pch.hpp"

#include "TSC/GLSLCodeGenerator.hpp"

namespace ProjectA {
	void GLSLCodeGenerator::write(std::ofstream& stream, const CompilationOutput& output) {
		this->output = &output;
		statementIndex = 0;
		statementByteOffset = 0;
		expressionReferenceIndex = 0;
		indentationLevel = 0;
		indentation = "";

		stream << "#version 310 es";

		while (statementIndex < this->output->ast.getStatementCount()) {
			stream << "\n";

			writeStatement(stream);
		}
	}

	void GLSLCodeGenerator::writeStatement(std::ofstream& stream) {
		const uint32 byteOffset = statementByteOffset;
		const StatementType type = output->ast.getStatementType(byteOffset);

		statementIndex += 1;

		switch (type) {
			case StatementType::STRUCTURE: {
				statementByteOffset += sizeof(Statement<StatementType::STRUCTURE>);

				writeStructureStatement(stream, output->ast.getStatement<StatementType::STRUCTURE>(byteOffset));

				break;
			}
			case StatementType::MEMBER: {
				statementByteOffset += sizeof(Statement<StatementType::MEMBER>);

				writeMemberStatement(stream, output->ast.getStatement<StatementType::MEMBER>(byteOffset));

				break;
			}
			case StatementType::CONSTANT: {
				statementByteOffset += sizeof(Statement<StatementType::CONSTANT>);

				writeConstantStatement(stream, output->ast.getStatement<StatementType::CONSTANT>(byteOffset));

				break;
			}
			case StatementType::VARIABLE: {
				statementByteOffset += sizeof(Statement<StatementType::VARIABLE>);

				writeVariableStatement(stream, output->ast.getStatement<StatementType::VARIABLE>(byteOffset));

				break;
			}
			case StatementType::PARAMETER: {
				statementByteOffset += sizeof(Statement<StatementType::PARAMETER>);

				writeParameterStatement(stream, output->ast.getStatement<StatementType::PARAMETER>(byteOffset));

				break;
			}
			case StatementType::FUNCTION: {
				statementByteOffset += sizeof(Statement<StatementType::FUNCTION>);

				writeFunctionStatement(stream, output->ast.getStatement<StatementType::FUNCTION>(byteOffset));

				break;
			}
			case StatementType::EXPRESSION: {
				statementByteOffset += sizeof(Statement<StatementType::EXPRESSION>);

				writeExpressionStatement(stream, output->ast.getStatement<StatementType::EXPRESSION>(byteOffset));

				break;
			}
			case StatementType::SCOPE: {
				statementByteOffset += sizeof(Statement<StatementType::SCOPE>);

				writeScopeStatement(stream, output->ast.getStatement<StatementType::SCOPE>(byteOffset));

				break;
			}
			case StatementType::IF: {
				statementByteOffset += sizeof(Statement<StatementType::IF>);

				writeIfStatement(stream, output->ast.getStatement<StatementType::IF>(byteOffset));

				break;
			}
			case StatementType::WHILE: {
				statementByteOffset += sizeof(Statement<StatementType::WHILE>);

				writeWhileStatement(stream, output->ast.getStatement<StatementType::WHILE>(byteOffset));

				break;
			}
			case StatementType::BREAK: {
				statementByteOffset += sizeof(Statement<StatementType::BREAK>);

				writeBreakStatement(stream, output->ast.getStatement<StatementType::BREAK>(byteOffset));

				break;
			}
			case StatementType::NEXT: {
				statementByteOffset += sizeof(Statement<StatementType::NEXT>);

				writeNextStatement(stream, output->ast.getStatement<StatementType::NEXT>(byteOffset));

				break;
			}
			case StatementType::RETURN: {
				statementByteOffset += sizeof(Statement<StatementType::RETURN>);

				writeReturnStatement(stream, output->ast.getStatement<StatementType::RETURN>(byteOffset));

				break;
			}
		}
	}

	void GLSLCodeGenerator::writeStructureStatement(std::ofstream& stream, const Statement<StatementType::STRUCTURE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		stream << indentation << "struct " << output->getTokenString(identifierToken) << " {\n";

		indent();

		uint32 memberIndex = 0;

		while (memberIndex < statement.memberCount) {
			writeStatement(stream);

			memberIndex += 1;
		}

		unindent();

		stream << "};";
	}

	void GLSLCodeGenerator::writeMemberStatement(std::ofstream& stream, const Statement<StatementType::MEMBER>& statement) {
		if (statement.annotationFlag & ENTRY_POINT_ANNOTATION_MASK) {
			expressionReferenceIndex += 1;

			return;
		}

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		stream << indentation;

		writeAnnotationBlocks(stream, statement.annotationBlockIndexOffset, statement.annotationBlockCount);
		writeExpression(stream);

		stream << " " << output->getTokenString(identifierToken) << ";\n";
	}

	void GLSLCodeGenerator::writeConstantStatement(std::ofstream& stream, const Statement<StatementType::CONSTANT>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writeAnnotationBlocks(stream, statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		stream << "const ";

		writeExpression(stream);

		stream << " " << output->getTokenString(identifierToken) << " = ";

		writeExpression(stream);

		stream << ";";
	}

	void GLSLCodeGenerator::writeVariableStatement(std::ofstream& stream, const Statement<StatementType::VARIABLE>& statement) {
		stream << indentation;

		if (statement.constant) {
			stream << "const ";
		}

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writeExpression(stream);

		stream << " ";
		stream << output->getTokenString(identifierToken);

		if (statement.initializer) {
			stream << " = ";

			writeExpression(stream);
		}

		stream << ";";
	}

	void GLSLCodeGenerator::writeParameterStatement(std::ofstream& stream, const Statement<StatementType::PARAMETER>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writeExpression(stream);

		stream << " " << output->getTokenString(identifierToken);
	}

	void GLSLCodeGenerator::writeFunctionStatement(std::ofstream& stream, const Statement<StatementType::FUNCTION>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		stream << indentation;

		writeExpression(stream);

		stream << " " << output->getTokenString(identifierToken) << "(";

		uint32 parameterIndex = 0;

		while (parameterIndex < statement.parameterCount) {
			writeStatement(stream);

			parameterIndex += 1;

			if (parameterIndex < statement.parameterCount) {
				stream << ", ";
			}
		}

		stream << ") ";

		writeStatement(stream);
	}

	void GLSLCodeGenerator::writeExpressionStatement(std::ofstream& stream, const Statement<StatementType::EXPRESSION>& statement) {
		stream << indentation;

		writeExpression(stream);

		stream << ";";
	}

	void GLSLCodeGenerator::writeScopeStatement(std::ofstream& stream, const Statement<StatementType::SCOPE>& statement) {
		stream << "{\n";

		indent();

		uint32 statementIndex = 0;

		while (statementIndex < statement.statementCount) {
			writeStatement(stream);

			stream << "\n";

			statementIndex += 1;
		}

		unindent();

		stream << indentation << "}";
	}

	void GLSLCodeGenerator::writeIfStatement(std::ofstream& stream, const Statement<StatementType::IF>& statement) {
		stream << indentation << "if (";

		writeExpression(stream);

		stream << ") ";

		// Scope
		writeStatement(stream);
	}

	void GLSLCodeGenerator::writeWhileStatement(std::ofstream& stream, const Statement<StatementType::WHILE>& statement) {
		stream << indentation << "while (";

		writeExpression(stream);

		stream << ") ";

		// Scope
		writeStatement(stream);
	}

	void GLSLCodeGenerator::writeBreakStatement(std::ofstream& stream, const Statement<StatementType::BREAK>& statement) {
		stream << indentation << "break;";
	}

	void GLSLCodeGenerator::writeNextStatement(std::ofstream& stream, const Statement<StatementType::NEXT>& statement) {
		stream << indentation << "continue;";
	}

	void GLSLCodeGenerator::writeReturnStatement(std::ofstream& stream, const Statement<StatementType::RETURN>& statement) {
		stream << indentation << "return ";

		writeExpression(stream);

		stream << ";";
	}

	void GLSLCodeGenerator::writeExpression(std::ofstream& stream) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(expressionReferenceIndex);

		expressionReferenceIndex += 1;

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: writeLiteralExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::IDENTIFIER: writeIdentifierExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::UNARY: writeUnaryExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::BINARY: writeBinaryExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::GROUP: writeGroupExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::MEMBER_ACCESS: writeMemberAccessExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::CALL: writeCallExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::TYPE: writeTypeExpression(stream, expressionReference.byteOffset); break;
		}
	}

	void GLSLCodeGenerator::writeLiteralExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		DEBUG_ASSERT(token.type == TokenType::LITERAL_INTEGER || token.type == TokenType::LITERAL_FLOATING_POINT);

		stream << output->getTokenString(token);
	}

	void GLSLCodeGenerator::writeIdentifierExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		stream << output->getTokenString(token);
	}

	void GLSLCodeGenerator::writeUnaryExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		stream << output->getNonIdentifierTokenString(operatorToken.type);

		writeExpression(stream);
	}

	void GLSLCodeGenerator::writeBinaryExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		writeExpression(stream);

		stream << " " << output->getNonIdentifierTokenString(operatorToken.type) << " ";

		writeExpression(stream);
	}

	void GLSLCodeGenerator::writeGroupExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(byteOffset);

		stream << "(";

		writeExpression(stream);

		stream << ")";
	}

	void GLSLCodeGenerator::writeMemberAccessExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(byteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		writeExpression(stream);

		stream << "." << output->getTokenString(identifierToken);
	}

	void GLSLCodeGenerator::writeCallExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(byteOffset);
		uint32 argumentIndex = 0;

		if (expression.explicitCallee) {
			writeExpression(stream);
		}

		stream << "(";

		while (argumentIndex < expression.argumentCount) {
			writeExpression(stream);

			argumentIndex += 1;

			if (argumentIndex < expression.argumentCount) {
				stream << ", ";
			}
		}

		stream << ")";
	}

	void GLSLCodeGenerator::writeTypeExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(byteOffset);

		stream << getGlslType(expression.typeTokenIndex, expression.templateArgumentTokenIndex);
	}

	void GLSLCodeGenerator::writeAnnotationBlocks(std::ofstream& stream, uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) {
		if (annotationBlockCount == 0) {
			return;
		}

		stream << "layout(";

		uint32 annotationBlockIndex = 0;

		while (annotationBlockIndex < annotationBlockCount) {
			const AnnotationBlock& annotationBlock = output->ast.getAnnotationBlock(annotationBlockIndexOffset + annotationBlockCount);

			annotationBlockIndex += 1;

			switch (annotationBlock.annotation) {
				case Annotation::LOCATION: {
					const Token& valueToken = output->tokens[annotationBlock.valueTokenIndex];

					stream << "location = " << output->getTokenString(valueToken) << "";

					break;
				}
				default: {
					// TODO
					BREAKPOINT();

					continue;
				}
			}

			if (annotationBlockIndex < annotationBlockCount) {
				stream << ", ";
			}
		}

		stream << ") ";
	}

	void GLSLCodeGenerator::indent() {
		indentationLevel += 1;
		indentation = std::string(indentationLevel, '\t');
	}

	void GLSLCodeGenerator::unindent() {
		DEBUG_ASSERT(indentationLevel > 0);

		indentationLevel -= 1;
		indentation = std::string(indentationLevel, '\t');
	}
}