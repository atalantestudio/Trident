#include "pch.hpp"

#include "TSC/ASTDebugger.hpp"

namespace ProjectA {
	ASTDebugger::ASTDebugger(std::string_view path, const CompilationOutput& output) :
		logger(path, 4)
	{
		this->output = &output;
		statementIndex = 0;
		statementByteOffset = 0;
		expressionReferenceIndex = 0;
		expressionByteOffset = 0;

		logger.openArray();

		while (statementIndex < output.ast.getStatementCount()) {
			logger.openObject();

			writeStatement();

			logger.closeObject();
		}

		logger.closeArray();
	}

	void ASTDebugger::writeStatement() {
		const uint32 byteOffset = statementByteOffset;
		const StatementType type = output->ast.getStatementType(byteOffset);

		statementIndex += 1;

		switch (type) {
			case StatementType::STRUCTURE: {
				statementByteOffset += sizeof(Statement<StatementType::STRUCTURE>);

				writeStructureStatement(output->ast.getStatement<StatementType::STRUCTURE>(byteOffset));

				break;
			}
			case StatementType::MEMBER: {
				statementByteOffset += sizeof(Statement<StatementType::MEMBER>);

				writeMemberStatement(output->ast.getStatement<StatementType::MEMBER>(byteOffset));

				break;
			}
			case StatementType::CONSTANT: {
				statementByteOffset += sizeof(Statement<StatementType::CONSTANT>);

				writeConstantStatement(output->ast.getStatement<StatementType::CONSTANT>(byteOffset));

				break;
			}
			case StatementType::VARIABLE: {
				statementByteOffset += sizeof(Statement<StatementType::VARIABLE>);

				writeVariableStatement(output->ast.getStatement<StatementType::VARIABLE>(byteOffset));

				break;
			}
			case StatementType::PARAMETER: {
				statementByteOffset += sizeof(Statement<StatementType::PARAMETER>);

				writeParameterStatement(output->ast.getStatement<StatementType::PARAMETER>(byteOffset));

				break;
			}
			case StatementType::FUNCTION: {
				statementByteOffset += sizeof(Statement<StatementType::FUNCTION>);

				writeFunctionStatement(output->ast.getStatement<StatementType::FUNCTION>(byteOffset));

				break;
			}
			case StatementType::EXPRESSION: {
				statementByteOffset += sizeof(Statement<StatementType::EXPRESSION>);

				writeExpressionStatement(output->ast.getStatement<StatementType::EXPRESSION>(byteOffset));

				break;
			}
			case StatementType::SCOPE: {
				statementByteOffset += sizeof(Statement<StatementType::SCOPE>);

				writeScopeStatement(output->ast.getStatement<StatementType::SCOPE>(byteOffset));

				break;
			}
			case StatementType::IF: {
				statementByteOffset += sizeof(Statement<StatementType::IF>);

				writeIfStatement(output->ast.getStatement<StatementType::IF>(byteOffset));

				break;
			}
			case StatementType::WHILE: {
				statementByteOffset += sizeof(Statement<StatementType::WHILE>);

				writeWhileStatement(output->ast.getStatement<StatementType::WHILE>(byteOffset));

				break;
			}
			case StatementType::BREAK: {
				statementByteOffset += sizeof(Statement<StatementType::BREAK>);

				writeBreakStatement(output->ast.getStatement<StatementType::BREAK>(byteOffset));

				break;
			}
			case StatementType::NEXT: {
				statementByteOffset += sizeof(Statement<StatementType::NEXT>);

				writeNextStatement(output->ast.getStatement<StatementType::NEXT>(byteOffset));

				break;
			}
			case StatementType::RETURN: {
				statementByteOffset += sizeof(Statement<StatementType::RETURN>);

				writeReturnStatement(output->ast.getStatement<StatementType::RETURN>(byteOffset));

				break;
			}
		}
	}

	void ASTDebugger::writeStructureStatement(const Statement<StatementType::STRUCTURE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("StatementType", "StructureStatement");
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyArray("Members");

		uint32 memberIndex = 0;

		while (memberIndex < statement.memberCount) {
			logger.openObject();

			writeStatement();

			logger.closeObject();

			memberIndex += 1;
		}

		logger.closeArray();
	}

	void ASTDebugger::writeMemberStatement(const Statement<StatementType::MEMBER>& statement) {
		logger.property("StatementType", "MemberStatement");
		logger.propertyNonString("AnnotationFlag", statement.annotationFlag);

		writeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		logger.propertyObject("Type");

		writeExpression();

		logger.closeObject();

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeConstantStatement(const Statement<StatementType::CONSTANT>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("StatementType", "ConstantStatement");
		logger.propertyNonString("AnnotationFlag", statement.annotationFlag);

		writeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		logger.propertyObject("Type");

		writeExpression();

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyObject("Initializer");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeVariableStatement(const Statement<StatementType::VARIABLE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("StatementType", "VariableStatement");
		logger.propertyObject("Type");

		writeExpression();

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyNonString("Constant", statement.constant);

		if (statement.initializer) {
			logger.propertyObject("Initializer");

			writeExpression();

			logger.closeObject();
		}
		else {
			logger.property("Initializer", "null");
		}
	}

	void ASTDebugger::writeParameterStatement(const Statement<StatementType::PARAMETER>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("StatementType", "ParameterStatement");
		logger.propertyObject("Type");

		writeExpression();

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeFunctionStatement(const Statement<StatementType::FUNCTION>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		logger.property("StatementType", "FunctionStatement");
		logger.propertyObject("ReturnType");

		writeExpression();

		logger.closeObject();
		logger.property("Identifier", output->getTokenString(identifierToken));
		logger.propertyArray("Arguments");

		uint32 parameterIndex = 0;

		while (parameterIndex < statement.parameterCount) {
			logger.openObject();

			writeStatement();

			logger.closeObject();

			parameterIndex += 1;
		}

		logger.closeArray();
		logger.propertyObject("Scope");

		writeStatement();

		logger.closeObject();
	}

	void ASTDebugger::writeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement) {
		logger.property("StatementType", "ExpressionStatement");
		logger.propertyObject("Expression");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeScopeStatement(const Statement<StatementType::SCOPE>& statement) {
		logger.property("StatementType", "ScopeStatement");
		logger.propertyArray("Statements");

		uint32 statementIndex = 0;

		while (statementIndex < statement.statementCount) {
			logger.openObject();

			writeStatement();

			logger.closeObject();

			statementIndex += 1;
		}

		logger.closeArray();
	}

	void ASTDebugger::writeIfStatement(const Statement<StatementType::IF>& statement) {
		logger.property("StatementType", "IfStatement");
		logger.propertyObject("Condition");

		writeExpression();

		logger.closeObject();
		logger.propertyObject("Scope");

		writeStatement();

		logger.closeObject();
	}

	void ASTDebugger::writeWhileStatement(const Statement<StatementType::WHILE>& statement) {
		logger.property("StatementType", "WhileStatement");
		logger.propertyObject("Condition");

		writeExpression();

		logger.closeObject();
		logger.propertyObject("Scope");

		writeStatement();

		logger.closeObject();
	}

	void ASTDebugger::writeBreakStatement(const Statement<StatementType::BREAK>& statement) {
		logger.property("StatementType", "BreakStatement");
	}

	void ASTDebugger::writeNextStatement(const Statement<StatementType::NEXT>& statement) {
		logger.property("StatementType", "NextStatement");
	}

	void ASTDebugger::writeReturnStatement(const Statement<StatementType::RETURN>& statement) {
		logger.property("StatementType", "ReturnStatement");
		logger.propertyObject("Expression");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeExpression() {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(expressionReferenceIndex);

		expressionReferenceIndex += 1;
		expressionByteOffset = expressionReference.byteOffset;

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: writeLiteralExpression(); break;
			case ExpressionType::IDENTIFIER: writeIdentifierExpression(); break;
			case ExpressionType::UNARY: writeUnaryExpression(); break;
			case ExpressionType::BINARY: writeBinaryExpression(); break;
			case ExpressionType::GROUP: writeGroupExpression(); break;
			case ExpressionType::MEMBER_ACCESS: writeMemberAccessExpression(); break;
			case ExpressionType::CALL: writeCallExpression(); break;
			case ExpressionType::TYPE: writeTypeExpression(); break;
		}
	}

	void ASTDebugger::writeLiteralExpression() {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(expressionByteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		logger.property("ExpressionType", "LiteralExpression");

		switch (token.type) {
			case TokenType::LITERAL_FLOATING_POINT: {
				logger.property("LiteralType", "FloatingPoint");
				logger.propertyNonString("Literal", std::stold(output->getTokenString(token)));

				break;
			}
			case TokenType::LITERAL_INTEGER: {
				logger.property("LiteralType", "Integer");
				logger.propertyNonString("Literal", std::stoull(output->getTokenString(token)));

				break;
			}
			default: {
				break;
			}
		}
	}

	void ASTDebugger::writeIdentifierExpression() {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(expressionByteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		logger.property("ExpressionType", "IdentifierExpression");
		logger.property("Identifier", output->getTokenString(token));
	}

	void ASTDebugger::writeUnaryExpression() {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		logger.property("ExpressionType", "UnaryExpression");
		logger.property("Operator", output->getTokenString(operatorToken));
		logger.propertyObject("Right");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeBinaryExpression() {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		logger.property("ExpressionType", "BinaryExpression");
		logger.propertyObject("Left");

		writeExpression();

		logger.closeObject();
		logger.property("Operator", output->getTokenString(operatorToken));
		logger.propertyObject("Right");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeGroupExpression() {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(expressionByteOffset);

		logger.property("ExpressionType", "GroupExpression");
		logger.propertyObject("Expression");

		writeExpression();

		logger.closeObject();
	}

	void ASTDebugger::writeMemberAccessExpression() {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(expressionByteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		logger.property("ExpressionType", "MemberAccessExpression");

		// TODO: Write parent expression.
		/* logger.propertyObject("Parent");

		writeExpression();

		logger.closeObject(); */

		logger.property("MemberIdentifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeCallExpression() {
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(expressionByteOffset);

		logger.property("ExpressionType", "CallExpression");

		if (expression.explicitCallee) {
			logger.propertyObject("Callee");

			writeExpression();

			logger.closeObject();
		}
		else {
			logger.property("Callee", "null");
		}

		logger.propertyNonString("ArgumentCount", expression.argumentCount);
		logger.propertyArray("Arguments");

		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			logger.openObject();

			writeExpression();

			logger.closeObject();

			argumentIndex += 1;
		}

		logger.closeArray();
	}

	void ASTDebugger::writeTypeExpression() {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionByteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		logger.property("ExpressionType", "TypeExpression");
		logger.propertyNonString("TypeIndex", expression.type.typeIndex);
		logger.propertyNonString("TemplateArgumentIndex", expression.type.templateArgumentIndex);
	}

	void ASTDebugger::writeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) {
		logger.propertyArray("Annotations");

		uint32 annotationBlockIndex = 0;

		while (annotationBlockIndex < annotationBlockCount) {
			const AnnotationBlock& annotationBlock = output->ast.getAnnotationBlock(annotationBlockIndexOffset + annotationBlockIndex);
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			logger.openObject();
			logger.property("Annotation", output->getTokenString(identifierToken));

			if (annotationBlock.valueTokenIndex != UNDEFINED_TOKEN) {
				const Token& valueToken = output->tokens[annotationBlock.valueTokenIndex];

				logger.propertyNonString("Value", std::stoull(output->getTokenString(valueToken)));
			}

			logger.closeObject();

			annotationBlockIndex += 1;
		}

		logger.closeArray();
	}
}