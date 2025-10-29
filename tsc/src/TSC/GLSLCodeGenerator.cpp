#include "pch.hpp"

#include "TSC/GLSLCodeGenerator.hpp"

namespace ProjectA {
	void GLSLCodeGenerator::write(std::ofstream& stream, const CompilationOutput& output) {
		this->output = &output;
		indentationLevel = 0;
		indentation = "";

		stream << "#version 310 es";

		for (const uint32 statementIndex : output.ast.globalScopeStatementIndices) {
			stream << "\n";

			writeStatement(stream, statementIndex);
		}
	}

	void GLSLCodeGenerator::writeStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<Statement>& statement = output->ast.statements[statementIndex];

		switch (statement->statementType) {
			case StatementType::STRUCTURE: writeStructureStatement(stream, statementIndex); break;
			case StatementType::MEMBER: writeMemberStatement(stream, statementIndex); break;
			case StatementType::CONSTANT: writeConstantStatement(stream, statementIndex); break;
			case StatementType::VARIABLE: writeVariableStatement(stream, statementIndex); break;
			case StatementType::PARAMETER: writeParameterStatement(stream, statementIndex); break;
			case StatementType::FUNCTION: writeFunctionStatement(stream, statementIndex); break;
			case StatementType::EXPRESSION: writeExpressionStatement(stream, statementIndex); break;
			case StatementType::SCOPE: writeScopeStatement(stream, statementIndex); break;
			case StatementType::IF: writeIfStatement(stream, statementIndex); break;
			case StatementType::WHILE: writeWhileStatement(stream, statementIndex); break;
			case StatementType::BREAK: writeBreakStatement(stream); break;
			case StatementType::NEXT: writeNextStatement(stream); break;
			case StatementType::RETURN: writeReturnStatement(stream, statementIndex); break;
		}
	}

	void GLSLCodeGenerator::writeStructureStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<StructureStatement> statement = std::static_pointer_cast<StructureStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << indentation << "struct " << output->getTokenString(identifierToken) << " {\n";

		indent();

		const uint32 memberStatementOffset = statementIndex + 1;

		for (uint32 memberIndex = 0; memberIndex < statement->memberCount; memberIndex += 1) {
			writeStatement(stream, memberStatementOffset + memberIndex);
		}

		unindent();

		stream << "};";
	}

	void GLSLCodeGenerator::writeMemberStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<MemberStatement> statement = std::static_pointer_cast<MemberStatement>(output->ast.statements[statementIndex]);

		if (statement->annotationFlag & (Annotation::INSTANCE_INDEX | Annotation::POSITION | Annotation::VERTEX_INDEX)) {
			return;
		}

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << indentation;

		writeAnnotationBlocks(stream, statement->annotationBlockIndexOffset, statement->annotationBlockCount);
		writeExpression(stream, statement->typeExpressionReferenceIndex);

		stream << " " << output->getTokenString(identifierToken) << ";\n";
	}

	void GLSLCodeGenerator::writeConstantStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ConstantStatement> statement = std::static_pointer_cast<ConstantStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		writeAnnotationBlocks(stream, statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		stream << "const ";

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		stream << " " << output->getTokenString(identifierToken) << " = ";

		writeExpression(stream, statement->initializerExpressionReferenceIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeVariableStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<VariableStatement> statement = std::static_pointer_cast<VariableStatement>(output->ast.statements[statementIndex]);

		stream << indentation;

		if (statement->constant) {
			stream << "const ";
		}

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		stream << " ";
		stream << output->getTokenString(identifierToken);

		if (statement->initializerExpressionReferenceIndex != UNDEFINED_EXPRESSION) {
			stream << " = ";

			writeExpression(stream, statement->initializerExpressionReferenceIndex);
		}

		stream << ";";
	}

	void GLSLCodeGenerator::writeParameterStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ParameterStatement> statement = std::static_pointer_cast<ParameterStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		stream << " " << output->getTokenString(identifierToken);
	}

	void GLSLCodeGenerator::writeFunctionStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << indentation;

		writeExpression(stream, statement->returnTypeExpressionReferenceIndex);

		stream << " " << output->getTokenString(identifierToken) << "(";

		for (uint32 statementIndexOffset = 0; statementIndexOffset < statement->parameterStatementIndices.size(); statementIndexOffset += 1) {
			writeStatement(stream, statement->parameterStatementIndices[statementIndexOffset]);

			if (statementIndexOffset + 1 < statement->parameterStatementIndices.size()) {
				stream << ", ";
			}
		}

		stream << ") ";

		writeStatement(stream, statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeExpressionStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ExpressionStatement> statement = std::static_pointer_cast<ExpressionStatement>(output->ast.statements[statementIndex]);

		stream << indentation;

		writeExpression(stream, statement->expressionReferenceIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeScopeStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ScopeStatement> statement = std::static_pointer_cast<ScopeStatement>(output->ast.statements[statementIndex]);

		stream << "{\n";

		indent();

		for (const uint32 statementIndex : statement->statementIndices) {
			writeStatement(stream, statementIndex);

			stream << "\n";
		}

		unindent();

		stream << indentation << "}";
	}

	void GLSLCodeGenerator::writeIfStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<IfStatement> statement = std::static_pointer_cast<IfStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "if (";

		writeExpression(stream, statement->conditionExpressionReferenceIndex);

		stream << ") ";

		writeScopeStatement(stream, statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeWhileStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<WhileStatement> statement = std::static_pointer_cast<WhileStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "while (";

		writeExpression(stream, statement->conditionExpressionReferenceIndex);

		stream << ") ";

		writeScopeStatement(stream, statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeBreakStatement(std::ofstream& stream) const {
		stream << indentation << "break;";
	}

	void GLSLCodeGenerator::writeNextStatement(std::ofstream& stream) const {
		stream << indentation << "continue;";
	}

	void GLSLCodeGenerator::writeReturnStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ReturnStatement> statement = std::static_pointer_cast<ReturnStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "return ";

		writeExpression(stream, statement->expressionReferenceIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeExpression(std::ofstream& stream, uint32 referenceIndex) {
		uint32 referenceCount = 0;

		writeExpression(stream, referenceIndex, referenceCount);
	}

	void GLSLCodeGenerator::writeExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: writeLiteralExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::IDENTIFIER: writeIdentifierExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::UNARY: writeUnaryExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::BINARY: writeBinaryExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::GROUP: writeGroupExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::MEMBER_ACCESS: writeMemberAccessExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::CALL: writeCallExpression(stream, referenceIndex, referenceCount); break;
			case ExpressionType::TYPE: writeTypeExpression(stream, referenceIndex, referenceCount); break;
		}
	}

	void GLSLCodeGenerator::writeLiteralExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(expressionReference.byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		DEBUG_ASSERT(token.type == TokenType::LITERAL_INTEGER || token.type == TokenType::LITERAL_FLOATING_POINT);

		stream << output->getTokenString(token);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeIdentifierExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(expressionReference.byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		stream << output->getTokenString(token);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeUnaryExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(expressionReference.byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		stream << output->getNonIdentifierTokenString(operatorToken.type);

		writeExpression(stream, expression.rightExpressionReferenceIndex, referenceCount);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeBinaryExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(expressionReference.byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		writeExpression(stream, expression.leftExpressionReferenceIndex, referenceCount);

		stream << " " << output->getNonIdentifierTokenString(operatorToken.type) << " ";

		writeExpression(stream, expression.rightExpressionReferenceIndex, referenceCount);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeGroupExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(expressionReference.byteOffset);

		stream << "(";

		writeExpression(stream, expression.expressionReferenceIndex, referenceCount);

		stream << ")";

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeMemberAccessExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(expressionReference.byteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		writeExpression(stream, expression.leftExpressionReferenceIndex, referenceCount);

		stream << "." << output->getTokenString(identifierToken);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeCallExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(expressionReference.byteOffset);

		writeExpression(stream, expression.calleeExpressionReferenceIndex, referenceCount);

		stream << "(";

		uint32 argumentExpressionReferenceIndex = referenceIndex - 1;
		uint32 argumentIndex = 0;

		// TODO: Write arguments in order (not reverse).
		while (argumentIndex < expression.argumentCount) {
			uint32 argumentExpressionReferenceOffset = 0;

			writeExpression(stream, argumentExpressionReferenceIndex, argumentExpressionReferenceOffset);

			argumentExpressionReferenceIndex -= argumentExpressionReferenceOffset;
			argumentIndex += 1;

			if (argumentIndex < expression.argumentCount) {
				stream << ", ";
			}
		}

		stream << ")";

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeTypeExpression(std::ofstream& stream, uint32 referenceIndex, uint32& referenceCount) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionReference.byteOffset);

		stream << getGlslType(expression.typeTokenIndex, expression.templateArgumentTokenIndex);

		referenceCount += 1;
	}

	void GLSLCodeGenerator::writeAnnotationBlocks(std::ofstream& stream, uint32 indexOffset, uint32 count) {
		if (count == 0) {
			return;
		}

		stream << "layout(";

		for (uint32 index = 0; index < count; index += 1) {
			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[indexOffset + index];

			switch (annotationBlock.annotation) {
				case Annotation::LOCATION: {
					const Token& valueToken = output->tokens[annotationBlock.valueTokenIndex];

					stream << "location = " << output->getTokenString(valueToken) << "";

					break;
				}
				default:
					// TODO
					break;
			}

			if (index + 1 < count) {
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