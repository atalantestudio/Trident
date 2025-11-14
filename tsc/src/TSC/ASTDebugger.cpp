#include "pch.hpp"

#include "TSC/ASTDebugger.hpp"

namespace ProjectA {
	ASTDebugger::ASTDebugger(view<char8> path, const CompilationOutput& output) :
		writer(path, 4)
	{
		this->output = &output;
		statementIndex = 0;
		statementByteOffset = 0;
		expressionReferenceIndex = 0;
		expressionByteOffset = 0;

		writer.openArray();

		while (statementIndex < output.ast.getStatementCount()) {
			writer.openObject();

			writeStatement();

			writer.closeObject();
		}

		writer.closeArray();
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

		writer.property("StatementType", "StructureStatement");
		writer.property("Identifier", output->getTokenString(identifierToken));
		writer.propertyArray("Members");

		uint32 memberIndex = 0;

		while (memberIndex < statement.memberCount) {
			writer.openObject();

			writeStatement();

			writer.closeObject();

			memberIndex += 1;
		}

		writer.closeArray();
	}

	void ASTDebugger::writeMemberStatement(const Statement<StatementType::MEMBER>& statement) {
		writer.property("StatementType", "MemberStatement");
		writer.propertyNonString("AnnotationFlag", statement.annotationFlag);

		writeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		writer.propertyObject("Type");

		writeExpression();

		writer.closeObject();

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writer.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeConstantStatement(const Statement<StatementType::CONSTANT>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writer.property("StatementType", "ConstantStatement");
		writer.propertyNonString("AnnotationFlag", statement.annotationFlag);

		writeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		writer.propertyObject("Type");

		writeExpression();

		writer.closeObject();
		writer.property("Identifier", output->getTokenString(identifierToken));
		writer.propertyObject("Initializer");

		writeExpression();

		writer.closeObject();
	}

	void ASTDebugger::writeVariableStatement(const Statement<StatementType::VARIABLE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writer.property("StatementType", "VariableStatement");
		writer.propertyObject("Type");

		writeExpression();

		writer.closeObject();
		writer.property("Identifier", output->getTokenString(identifierToken));
		writer.propertyNonString("Constant", statement.constant);

		if (statement.initializer) {
			writer.propertyObject("Initializer");

			writeExpression();

			writer.closeObject();
		}
		else {
			writer.propertyNonString("Initializer", "null");
		}
	}

	void ASTDebugger::writeParameterStatement(const Statement<StatementType::PARAMETER>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writer.property("StatementType", "ParameterStatement");
		writer.propertyObject("Type");

		writeExpression();

		writer.closeObject();
		writer.property("Identifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeFunctionStatement(const Statement<StatementType::FUNCTION>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		writer.property("StatementType", "FunctionStatement");
		writer.propertyObject("ReturnType");

		writeExpression();

		writer.closeObject();
		writer.property("Identifier", output->getTokenString(identifierToken));
		writer.propertyArray("Arguments");

		uint32 parameterIndex = 0;

		while (parameterIndex < statement.parameterCount) {
			writer.openObject();

			writeStatement();

			writer.closeObject();

			parameterIndex += 1;
		}

		writer.closeArray();
		writer.propertyObject("Scope");

		writeStatement();

		writer.closeObject();
	}

	void ASTDebugger::writeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement) {
		writer.property("StatementType", "ExpressionStatement");
		writer.propertyObject("Expression");

		writeExpression();

		writer.closeObject();
	}

	void ASTDebugger::writeScopeStatement(const Statement<StatementType::SCOPE>& statement) {
		writer.property("StatementType", "ScopeStatement");
		writer.propertyArray("Statements");

		uint32 statementIndex = 0;

		while (statementIndex < statement.statementCount) {
			writer.openObject();

			writeStatement();

			writer.closeObject();

			statementIndex += 1;
		}

		writer.closeArray();
	}

	void ASTDebugger::writeIfStatement(const Statement<StatementType::IF>& statement) {
		writer.property("StatementType", "IfStatement");
		writer.propertyObject("Condition");

		writeExpression();

		writer.closeObject();
		writer.propertyObject("Scope");

		writeStatement();

		writer.closeObject();
	}

	void ASTDebugger::writeWhileStatement(const Statement<StatementType::WHILE>& statement) {
		writer.property("StatementType", "WhileStatement");
		writer.propertyObject("Condition");

		writeExpression();

		writer.closeObject();
		writer.propertyObject("Scope");

		writeStatement();

		writer.closeObject();
	}

	void ASTDebugger::writeBreakStatement(const Statement<StatementType::BREAK>& statement) {
		writer.property("StatementType", "BreakStatement");
	}

	void ASTDebugger::writeNextStatement(const Statement<StatementType::NEXT>& statement) {
		writer.property("StatementType", "NextStatement");
	}

	void ASTDebugger::writeReturnStatement(const Statement<StatementType::RETURN>& statement) {
		writer.property("StatementType", "ReturnStatement");
		writer.propertyObject("Expression");

		writeExpression();

		writer.closeObject();
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

		writer.property("ExpressionType", "LiteralExpression");

		switch (token.type) {
			case TokenType::LITERAL_FLOATING_POINT: {
				writer.property("LiteralType", "FloatingPoint");
				writer.propertyNonString("Literal", std::stold(output->getTokenString(token)));

				break;
			}
			case TokenType::LITERAL_INTEGER: {
				writer.property("LiteralType", "Integer");
				writer.propertyNonString("Literal", std::stoull(output->getTokenString(token)));

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

		writer.property("ExpressionType", "IdentifierExpression");
		writer.property("Identifier", output->getTokenString(token));
	}

	void ASTDebugger::writeUnaryExpression() {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		writer.property("ExpressionType", "UnaryExpression");
		writer.property("Operator", output->getTokenString(operatorToken));
		writer.propertyObject("Right");

		writeExpression();

		writer.closeObject();
	}

	void ASTDebugger::writeBinaryExpression() {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		writer.property("ExpressionType", "BinaryExpression");
		writer.propertyObject("Left");

		writeExpression();

		writer.closeObject();
		writer.property("Operator", output->getTokenString(operatorToken));
		writer.propertyObject("Right");

		writeExpression();

		writer.closeObject();
	}

	void ASTDebugger::writeGroupExpression() {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(expressionByteOffset);

		writer.property("ExpressionType", "GroupExpression");
		writer.propertyObject("Expression");

		writeExpression();

		writer.closeObject();
	}

	void ASTDebugger::writeMemberAccessExpression() {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(expressionByteOffset);
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];

		writer.property("ExpressionType", "MemberAccessExpression");

		// TODO: Write parent expression.
		/* writer.propertyObject("Parent");

		writeExpression();

		writer.closeObject(); */

		writer.property("MemberIdentifier", output->getTokenString(identifierToken));
	}

	void ASTDebugger::writeCallExpression() {
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(expressionByteOffset);

		writer.property("ExpressionType", "CallExpression");

		if (expression.explicitCallee) {
			writer.propertyObject("Callee");

			writeExpression();

			writer.closeObject();
		}
		else {
			writer.propertyNonString("Callee", "null");
		}

		writer.propertyNonString("ArgumentCount", expression.argumentCount);
		writer.propertyArray("Arguments");

		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			writer.openObject();

			writeExpression();

			writer.closeObject();

			argumentIndex += 1;
		}

		writer.closeArray();
	}

	void ASTDebugger::writeTypeExpression() {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionByteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		writer.property("ExpressionType", "TypeExpression");
		writer.property("TypeIdentifier", output->getTokenString(typeToken));

		if (expression.templateArgumentTokenIndex != ~0) {
			const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

			writer.property("TemplateArgumentIdentifier", output->getTokenString(templateArgumentToken));
		}
		else {
			writer.propertyNonString("TemplateArgumentIdentifier", "null");
		}
	}

	void ASTDebugger::writeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) {
		writer.propertyArray("Annotations");

		uint32 annotationBlockIndex = 0;

		while (annotationBlockIndex < annotationBlockCount) {
			const AnnotationBlock& annotationBlock = output->ast.getAnnotationBlock(annotationBlockIndexOffset + annotationBlockIndex);
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			writer.openObject();
			writer.property("Annotation", output->getTokenString(identifierToken));

			if (annotationBlock.valueTokenIndex != ~0) {
				const Token& valueToken = output->tokens[annotationBlock.valueTokenIndex];

				writer.propertyNonString("Value", std::stoull(output->getTokenString(valueToken)));
			}

			writer.closeObject();

			annotationBlockIndex += 1;
		}

		writer.closeArray();
	}
}