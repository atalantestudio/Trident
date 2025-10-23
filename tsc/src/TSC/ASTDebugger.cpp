#include "pch.hpp"

#include "TSC/ASTDebugger.hpp"

namespace ProjectA {
	void ASTDebugger::write(std::ofstream& stream, const CompilationOutput& output) {
		this->output = &output;
		indentationLevel = 0;
		indentation = "";

		stream << "[\n";

		indent();

		for (uint32 statementIndexOffset = 0; statementIndexOffset < output.ast.globalScopeStatementIndices.size(); statementIndexOffset += 1) {
			stream << indentation;

			const uint32 statementIndex = output.ast.globalScopeStatementIndices[statementIndexOffset];

			writeStatement(stream, statementIndex);

			if (statementIndexOffset + 1 < output.ast.globalScopeStatementIndices.size()) {
				stream << ",";
			}

			stream << "\n";
		}

		unindent();

		stream << "]";
	}

	void ASTDebugger::writeStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<Statement>& statement = output->ast.statements[statementIndex];

		stream << "{\n";

		indent();

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

		unindent();

		stream << "\n";
		stream << indentation << "}";
	}

	void ASTDebugger::writeStructureStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<StructureStatement> statement = std::static_pointer_cast<StructureStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << indentation << "\"Type\": \"StructureStatement\",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
		stream << indentation << "\"Members\": [\n";

		indent();

		for (uint32 statementIndexOffset = 0; statementIndexOffset < statement->memberStatementIndices.size(); statementIndexOffset += 1) {
			stream << indentation;

			writeStatement(stream, statement->memberStatementIndices[statementIndexOffset]);

			if (statementIndexOffset + 1 < statement->memberStatementIndices.size()) {
				stream << ",";
			}

			stream << "\n";
		}

		unindent();

		stream << indentation << "]";
	}

	void ASTDebugger::writeMemberStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<MemberStatement> statement = std::static_pointer_cast<MemberStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"MemberStatement\",\n";
		stream << indentation << "\"AnnotationFlag\": " << statement->annotationFlag << ",\n";
		stream << indentation << "\"Annotations\": ";

		writeAnnotationBlocks(stream, statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		stream << ",\n";
		stream << indentation << "\"_Type\": ";

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << ",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";
	}

	void ASTDebugger::writeConstantStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ConstantStatement> statement = std::static_pointer_cast<ConstantStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"ConstantStatement\",\n";
		stream << indentation << "\"AnnotationFlag\": " << statement->annotationFlag << ",\n";
		stream << indentation << "\"Annotations\": ";

		writeAnnotationBlocks(stream, statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		stream << ",\n";
		stream << indentation << "\"_Type\": ";

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << ",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
		stream << indentation << "\"Initializer\": ";

		writeExpression(stream, statement->initializerExpressionReferenceIndex);
	}

	void ASTDebugger::writeVariableStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<VariableStatement> statement = std::static_pointer_cast<VariableStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"VariableStatement\",\n";
		stream << indentation << "\"_Type\": ";

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << ",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
		stream << indentation << "\"Constant\": \"" << std::to_string(statement->constant) << "\",\n";
		stream << indentation << "\"Initializer\": ";

		if (statement->initializerExpressionReferenceIndex != UNDEFINED_EXPRESSION) {
			writeExpression(stream, statement->initializerExpressionReferenceIndex);

			return;
		}

		stream << "null";
	}

	void ASTDebugger::writeParameterStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ParameterStatement> statement = std::static_pointer_cast<ParameterStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"ParameterStatement\",\n";
		stream << indentation << "\"_Type\": ";

		writeExpression(stream, statement->typeExpressionReferenceIndex);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << ",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";
	}

	void ASTDebugger::writeFunctionStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		stream << indentation << "\"Type\": \"FunctionStatement\",\n";
		stream << indentation << "\"ReturnType\": ";

		writeExpression(stream, statement->returnTypeExpressionReferenceIndex);

		stream << ",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
		stream << indentation << "\"Arguments\": [\n";

		indent();

		for (uint32 statementIndexOffset = 0; statementIndexOffset < statement->parameterStatementIndices.size(); statementIndexOffset += 1) {
			stream << indentation;

			writeStatement(stream, statement->parameterStatementIndices[statementIndexOffset]);

			if (statementIndexOffset + 1 < statement->parameterStatementIndices.size()) {
				stream << ",";
			}

			stream << "\n";
		}

		unindent();

		stream << indentation << "],\n";
		stream << indentation << "\"Scope\": ";

		writeStatement(stream, statement->scopeStatementIndex);
	}

	void ASTDebugger::writeExpressionStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ExpressionStatement> statement = std::static_pointer_cast<ExpressionStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"ExpressionStatement\",\n";
		stream << indentation << "\"Expression\": ";

		writeExpression(stream, statement->expressionReferenceIndex);
	}

	void ASTDebugger::writeScopeStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ScopeStatement> statement = std::static_pointer_cast<ScopeStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"ScopeStatement\",\n";
		stream << indentation << "\"Statements\": [\n";

		indent();

		for (uint32 statementIndexOffset = 0; statementIndexOffset < statement->statementIndices.size(); statementIndexOffset += 1) {
			stream << indentation;

			writeStatement(stream, statement->statementIndices[statementIndexOffset]);

			if (statementIndexOffset + 1 < statement->statementIndices.size()) {
				stream << ",";
			}

			stream << "\n";
		}

		unindent();

		stream << indentation << "]";
	}

	void ASTDebugger::writeIfStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<IfStatement> statement = std::static_pointer_cast<IfStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"IfStatement\",\n";
		stream << indentation << "\"Condition\": ";

		writeExpression(stream, statement->conditionExpressionReferenceIndex);

		stream << ",\n";
		stream << indentation << "\"Scope\": ";

		writeStatement(stream, statement->scopeStatementIndex);
	}

	void ASTDebugger::writeWhileStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<WhileStatement> statement = std::static_pointer_cast<WhileStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"WhileStatement\",\n";
		stream << indentation << "\"Condition\": ";

		writeExpression(stream, statement->conditionExpressionReferenceIndex);

		stream << ",\n";
		stream << indentation << "\"Scope\": ";

		writeStatement(stream, statement->scopeStatementIndex);
	}

	void ASTDebugger::writeBreakStatement(std::ofstream& stream) const {
		stream << indentation << "\"Type\": \"BreakStatement\"";
	}

	void ASTDebugger::writeNextStatement(std::ofstream& stream) const {
		stream << indentation << "\"Type\": \"NextStatement\"";
	}

	void ASTDebugger::writeReturnStatement(std::ofstream& stream, uint32 statementIndex) {
		const ref<ReturnStatement> statement = std::static_pointer_cast<ReturnStatement>(output->ast.statements[statementIndex]);

		stream << indentation << "\"Type\": \"ReturnStatement\",\n";
		stream << indentation << "\"Expression\": ";

		writeExpression(stream, statement->expressionReferenceIndex);
	}

	void ASTDebugger::writeExpression(std::ofstream& stream, uint32 referenceIndex) {
		stream << "{\n";

		indent();

		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);

		switch (expressionReference.type) {
			case ExpressionType::NUMBER: writeNumberExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::IDENTIFIER: writeIdentifierExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::UNARY: writeUnaryExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::BINARY: writeBinaryExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::GROUP: writeGroupExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::MEMBER_ACCESS: writeMemberAccessExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::FUNCTION_CALL: writeFunctionCallExpression(stream, expressionReference.byteOffset); break;
			case ExpressionType::TYPE: writeTypeExpression(stream, expressionReference.byteOffset); break;
		}

		unindent();

		stream << "\n" << indentation << "}";
	}

	void ASTDebugger::writeNumberExpression(std::ofstream& stream, uint32 byteOffset) const {
		const Expression<ExpressionType::NUMBER>& expression = output->ast.getExpression<ExpressionType::NUMBER>(byteOffset);
		const Token& numberToken = output->tokens[expression.numberTokenIndex];

		stream << indentation << "\"Type\": \"NumberExpression\",\n";
		stream << indentation << "\"Number\": \"" << getTokenString(numberToken) << "\"";
	}

	void ASTDebugger::writeIdentifierExpression(std::ofstream& stream, uint32 byteOffset) const {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(byteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		stream << indentation << "\"Type\": \"IdentifierExpression\",\n";
		stream << indentation << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";
	}

	void ASTDebugger::writeUnaryExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		stream << indentation << "\"Type\": \"UnaryExpression\",\n";
		stream << indentation << "\"Operator\": \"" << getTokenString(operatorToken) << ",\n";
		stream << indentation << "\"Right\": ";

		writeExpression(stream, expression.rightExpressionReferenceIndex);
	}

	void ASTDebugger::writeBinaryExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		stream << indentation << "\"Type\": \"BinaryExpression\",\n";
		stream << indentation << "\"Left\": ";

		writeExpression(stream, expression.leftExpressionReferenceIndex);

		stream << ",\n";
		stream << indentation << "\"Operator\": \"" << getTokenString(operatorToken) << "\",\n";
		stream << indentation << "\"Right\": ";

		writeExpression(stream, expression.rightExpressionReferenceIndex);
	}

	void ASTDebugger::writeGroupExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(byteOffset);

		stream << indentation << "\"Type\": \"Expression<ExpressionType::GROUP>\",\n";
		stream << indentation << "\"Expression\": ";

		writeExpression(stream, expression.expressionReferenceIndex);
	}

	void ASTDebugger::writeMemberAccessExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(byteOffset);
		const Token& identifierTokenIndex = output->tokens[expression.identifierTokenIndex];

		stream << indentation << "\"Type\": \"MemberAccessExpression\",\n";
		stream << indentation << "\"Left\": ";

		writeExpression(stream, expression.leftExpressionReferenceIndex);

		stream << indentation << "\"MemberIdentifier\": \"" << getTokenString(identifierTokenIndex) << "\"";
	}

	void ASTDebugger::writeFunctionCallExpression(std::ofstream& stream, uint32 byteOffset) {
		const Expression<ExpressionType::FUNCTION_CALL>& expression = output->ast.getExpression<ExpressionType::FUNCTION_CALL>(byteOffset);

		stream << indentation << "\"Type\": \"FunctionCallExpression\",\n";
		stream << indentation << "\"Callee\": ";

		writeExpression(stream, expression.calleeExpressionReferenceIndex);

		stream << indentation << "\"Arguments\": [\n";

		indent();

		for (const uint32 expressionReferenceIndex : expression.argumentExpressionReferenceIndices) {
			stream << indentation;

			writeExpression(stream, expressionReferenceIndex);

			// TODO: Add trailing comma?
		}

		unindent();

		stream << indentation << "]";
	}

	void ASTDebugger::writeTypeExpression(std::ofstream& stream, uint32 byteOffset) const {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(byteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		stream << indentation << "\"Type\": \"TypeExpression\",\n";
		stream << indentation << "\"_Type\": \"" << getTokenString(typeToken) << "\",\n";
		stream << indentation << "\"TemplateArgument\": ";

		if (expression.templateArgumentTokenIndex != UNDEFINED_TOKEN) {
			const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

			stream << "\"" << getTokenString(templateArgumentToken) << "\"";

			return;
		}

		stream << "null";
	}

	void ASTDebugger::writeAnnotationBlocks(std::ofstream& stream, uint32 indexOffset, uint32 count) {
		stream << "[\n";

		indent();

		for (uint32 index = 0; index < count; index += 1) {
			stream << indentation << "{\n";

			indent();

			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[indexOffset + index];
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			stream << indentation << "\"Annotation\": \"" << getTokenString(identifierToken) << "\"";

			if (annotationBlock.valueTokenIndex != UNDEFINED_ANNOTATION_VALUE) {
				const Token& valueTokenIndex = output->tokens[annotationBlock.valueTokenIndex];

				stream << ",\n";
				stream << indentation << "\"Value\": \"" << getTokenString(valueTokenIndex) << "\"";
			}

			stream << "\n";

			unindent();

			stream << indentation << "}";

			if (index + 1 < count) {
				stream << ",";
			}

			stream << "\n";
		}

		unindent();

		stream << indentation << "]";
	}

	void ASTDebugger::indent() {
		indentationLevel += 1;
		indentation = std::string(indentationLevel, '\t');
	}

	void ASTDebugger::unindent() {
		DEBUG_ASSERT(indentationLevel > 0);

		indentationLevel -= 1;
		indentation = std::string(indentationLevel, '\t');
	}
}