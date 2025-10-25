#include "pch.hpp"

#include "TSC/ASTDebugger.hpp"

namespace ProjectA {
	ASTDebugger::ASTDebugger(std::string_view path, const CompilationOutput& output) :
		logger(path, 4)
	{
		this->output = &output;

		logger.openArray();

		for (const uint32 statementIndex : output.ast.globalScopeStatementIndices) {
			logger.openObject();

			writeStatement(statementIndex);

			logger.closeObject();
		}

		logger.closeArray();
	}

	void ASTDebugger::writeStatement(uint32 statementIndex) {
		const ref<Statement>& statement = output->ast.statements[statementIndex];

		switch (statement->statementType) {
			case StatementType::STRUCTURE: writeStructureStatement(statementIndex); break;
			case StatementType::MEMBER: writeMemberStatement(statementIndex); break;
			case StatementType::CONSTANT: writeConstantStatement(statementIndex); break;
			case StatementType::VARIABLE: writeVariableStatement(statementIndex); break;
			case StatementType::PARAMETER: writeParameterStatement(statementIndex); break;
			case StatementType::FUNCTION: writeFunctionStatement(statementIndex); break;
			case StatementType::EXPRESSION: writeExpressionStatement(statementIndex); break;
			case StatementType::SCOPE: writeScopeStatement(statementIndex); break;
			case StatementType::IF: writeIfStatement(statementIndex); break;
			case StatementType::WHILE: writeWhileStatement(statementIndex); break;
			case StatementType::BREAK: writeBreakStatement(); break;
			case StatementType::NEXT: writeNextStatement(); break;
			case StatementType::RETURN: writeReturnStatement(statementIndex); break;
		}
	}

	void ASTDebugger::writeStructureStatement(uint32 statementIndex) {
		const ref<StructureStatement> statement = std::static_pointer_cast<StructureStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Type", "StructureStatement");
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyArray("Members");

		for (const uint32 statementIndex : statement->memberStatementIndices) {
			logger.openObject();

			writeStatement(statementIndex);

			logger.closeObject();
		}

		logger.closeArray();
	}

	void ASTDebugger::writeMemberStatement(uint32 statementIndex) {
		const ref<MemberStatement> statement = std::static_pointer_cast<MemberStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "MemberStatement");
		logger.property("AnnotationFlag", statement->annotationFlag);

		writeAnnotationBlocks(statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		logger.propertyObject("_Type");

		writeExpression(statement->typeExpressionReferenceIndex);

		logger.closeObject();

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeConstantStatement(uint32 statementIndex) {
		const ref<ConstantStatement> statement = std::static_pointer_cast<ConstantStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Type", "ConstantStatement");
		logger.property("AnnotationFlag", statement->annotationFlag);

		writeAnnotationBlocks(statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		logger.propertyObject("_Type");

		writeExpression(statement->typeExpressionReferenceIndex);

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyObject("Initializer");

		writeExpression(statement->initializerExpressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeVariableStatement(uint32 statementIndex) {
		const ref<VariableStatement> statement = std::static_pointer_cast<VariableStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Type", "VariableStatement");
		logger.propertyObject("_Type");

		writeExpression(statement->typeExpressionReferenceIndex);

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.property("Constant", statement->constant);

		if (statement->initializerExpressionReferenceIndex != UNDEFINED_EXPRESSION) {
			logger.propertyObject("Initializer");

			writeExpression(statement->initializerExpressionReferenceIndex);

			logger.closeObject();
		}
		else {
			logger.property("Initializer", "null");
		}
	}

	void ASTDebugger::writeParameterStatement(uint32 statementIndex) {
		const ref<ParameterStatement> statement = std::static_pointer_cast<ParameterStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Type", "ParameterStatement");
		logger.propertyObject("_Type");

		writeExpression(statement->typeExpressionReferenceIndex);

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeFunctionStatement(uint32 statementIndex) {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		logger.property("Type", "FunctionStatement");
		logger.propertyObject("ReturnType");

		writeExpression(statement->returnTypeExpressionReferenceIndex);

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyArray("Arguments");

		for (const uint32 statementIndex : statement->parameterStatementIndices) {
			logger.openObject();

			writeStatement(statementIndex);

			logger.closeObject();
		}

		logger.closeArray();
		logger.propertyObject("Scope");

		writeStatement(statement->scopeStatementIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeExpressionStatement(uint32 statementIndex) {
		const ref<ExpressionStatement> statement = std::static_pointer_cast<ExpressionStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "ExpressionStatement");
		logger.propertyObject("Expression");

		writeExpression(statement->expressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeScopeStatement(uint32 statementIndex) {
		const ref<ScopeStatement> statement = std::static_pointer_cast<ScopeStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "ScopeStatement");
		logger.propertyArray("Statements");

		for (const uint32 statementIndex : statement->statementIndices) {
			logger.openObject();

			writeStatement(statementIndex);

			logger.closeObject();
		}

		logger.closeArray();
	}

	void ASTDebugger::writeIfStatement(uint32 statementIndex) {
		const ref<IfStatement> statement = std::static_pointer_cast<IfStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "IfStatement");
		logger.propertyObject("Condition");

		writeExpression(statement->conditionExpressionReferenceIndex);

		logger.closeObject();
		logger.propertyObject("Scope");

		writeStatement(statement->scopeStatementIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeWhileStatement(uint32 statementIndex) {
		const ref<WhileStatement> statement = std::static_pointer_cast<WhileStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "WhileStatement");
		logger.propertyObject("Condition");

		writeExpression(statement->conditionExpressionReferenceIndex);

		logger.closeObject();
		logger.propertyObject("Scope");

		writeStatement(statement->scopeStatementIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeBreakStatement() {
		logger.property("Type", "BreakStatement");
	}

	void ASTDebugger::writeNextStatement() {
		logger.property("Type", "NextStatement");
	}

	void ASTDebugger::writeReturnStatement(uint32 statementIndex) {
		const ref<ReturnStatement> statement = std::static_pointer_cast<ReturnStatement>(output->ast.statements[statementIndex]);

		logger.property("Type", "ReturnStatement");
		logger.propertyObject("Expression");

		writeExpression(statement->expressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeExpression(uint32 referenceIndex) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: writeLiteralExpression(expressionReference.byteOffset); break;
			case ExpressionType::IDENTIFIER: writeIdentifierExpression(expressionReference.byteOffset); break;
			case ExpressionType::UNARY: writeUnaryExpression(expressionReference.byteOffset); break;
			case ExpressionType::BINARY: writeBinaryExpression(expressionReference.byteOffset); break;
			case ExpressionType::GROUP: writeGroupExpression(expressionReference.byteOffset); break;
			case ExpressionType::MEMBER_ACCESS: writeMemberAccessExpression(expressionReference.byteOffset); break;
			case ExpressionType::FUNCTION_CALL: writeFunctionCallExpression(expressionReference.byteOffset); break;
			case ExpressionType::TYPE: writeTypeExpression(expressionReference.byteOffset); break;
		}
	}

	void ASTDebugger::writeLiteralExpression(uint32 byteOffset) {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		logger.property("Type", "LiteralExpression");

		switch (token.type) {
			case TokenType::LITERAL_FLOATING_POINT:
				logger.property("Literal", std::stof(output->getTokenString(token)));

				break;
			case TokenType::LITERAL_INTEGER_BASE_2:
			case TokenType::LITERAL_INTEGER_BASE_8:
			case TokenType::LITERAL_INTEGER_BASE_10:
			case TokenType::LITERAL_INTEGER_BASE_16:
				logger.property("Literal", std::stoi(output->getTokenString(token)));

				break;
			default:
				break;
		}
	}

	void ASTDebugger::writeIdentifierExpression(uint32 byteOffset) {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		logger.property("Type", "IdentifierExpression");
		logger.property("Identifier", output->getTokenString(token));
	}

	void ASTDebugger::writeUnaryExpression(uint32 byteOffset) {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		logger.property("Type", "UnaryExpression");
		logger.property("Operator", output->getTokenString(operatorToken));
		logger.propertyObject("Right");

		writeExpression(expression.rightExpressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeBinaryExpression(uint32 byteOffset) {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		logger.property("Type", "BinaryExpression");
		logger.propertyObject("Left");

		writeExpression(expression.leftExpressionReferenceIndex);

		logger.closeObject();
		logger.property("Operator", output->getTokenString(operatorToken));
		logger.propertyObject("Right");

		writeExpression(expression.rightExpressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeGroupExpression(uint32 byteOffset) {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(byteOffset);

		logger.property("Type", "GroupExpression");
		logger.propertyObject("Expression");

		writeExpression(expression.expressionReferenceIndex);

		logger.closeObject();
	}

	void ASTDebugger::writeMemberAccessExpression(uint32 byteOffset) {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(byteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		logger.property("Type", "MemberAccessExpression");
		logger.propertyObject("Left");

		writeExpression(expression.leftExpressionReferenceIndex);

		logger.closeObject();
		logger.property("MemberIdentifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeFunctionCallExpression(uint32 byteOffset) {
		const Expression<ExpressionType::FUNCTION_CALL>& expression = output->ast.getExpression<ExpressionType::FUNCTION_CALL>(byteOffset);

		logger.property("Type", "FunctionCallExpression");
		logger.propertyObject("Callee");

		writeExpression(expression.calleeExpressionReferenceIndex);

		logger.closeObject();
		logger.propertyArray("Arguments");

		for (const uint32 expressionReferenceIndex : expression.argumentExpressionReferenceIndices) {
			logger.openObject();

			writeExpression(expressionReferenceIndex);

			logger.closeObject();
		}

		logger.closeArray();
	}

	void ASTDebugger::writeTypeExpression(uint32 byteOffset) {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(byteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		logger.property("Type", "TypeExpression");
		logger.property("_Type", output->getTokenString(typeToken));

		std::string templateArgument = "null";

		if (expression.templateArgumentTokenIndex != UNDEFINED_TOKEN) {
			const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

			templateArgument = output->getTokenString(templateArgumentToken);
		}

		logger.property("TemplateArgument", templateArgument);
	}

	void ASTDebugger::writeAnnotationBlocks(uint32 indexOffset, uint32 count) {
		logger.propertyArray("Annotations");

		for (uint32 index = 0; index < count; index += 1) {
			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[indexOffset + index];
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			logger.openObject();
			logger.property("Annotation", output->getTokenString(identifierToken));

			if (annotationBlock.valueTokenIndex != UNDEFINED_ANNOTATION_VALUE) {
				const Token& valueToken = output->tokens[annotationBlock.valueTokenIndex];

				logger.property("Value", std::stoul(output->getTokenString(valueToken)));
			}

			logger.closeObject();
		}

		logger.closeArray();
	}
}