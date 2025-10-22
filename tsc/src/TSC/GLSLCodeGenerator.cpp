#include "pch.hpp"

#include "GLSLCodeGenerator.hpp"

namespace ProjectA {
	void GLSLCodeGenerator::write(const char8* path, const CompilationResult& result, const AST& ast) {
		stream.open(path);

		DEBUG_ASSERT(stream.is_open());

		this->result = &result;
		this->ast = &ast;

		stream << "#version 310 es";

		for (const uint32 globalScopeStatementIndex : this->ast->globalScopeStatementIndices) {
			stream << "\n";

			writeStatement(globalScopeStatementIndex);
		}

		stream.close();
	}

	void GLSLCodeGenerator::writeStatement(uint32 statementIndex) {
		const ref<Statement> statement = ast->statements[statementIndex];

		switch (statement->statementType) {
			case StatementType::GLOBAL_SCOPE_CONSTANT:
				return writeGlobalScopeConstant(statementIndex);
			case StatementType::STRUCTURE:
				return writeStructureStatement(statementIndex);
			case StatementType::FUNCTION:
				return writeFunctionStatement(statementIndex);
			case StatementType::VARIABLE:
				return writeVariableStatement(statementIndex);
			case StatementType::SCOPE:
				return writeScopeStatement(statementIndex);
			case StatementType::IF:
				return writeIfStatement(statementIndex);
			case StatementType::WHILE:
				return writeWhileStatement(statementIndex);
			case StatementType::BREAK:
				return writeBreakStatement();
			case StatementType::NEXT:
				return writeNextStatement();
			case StatementType::RETURN:
				return writeReturnStatement(statementIndex);
			case StatementType::EXPRESSION:
				return writeExpressionStatement(statementIndex);
			default:
				// TODO
				BREAKPOINT();

				break;
		}
	}

	void GLSLCodeGenerator::writeGlobalScopeConstant(uint32 statementIndex) {
		const ref<GlobalScopeConstantStatement> statement = std::static_pointer_cast<GlobalScopeConstantStatement>(ast->statements[statementIndex]);

		writeAnnotations(statement->annotationInstanceIndexOffset, statement->annotationInstanceCount);

		stream << "const ";

		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << " = ";

		writeExpression(statement->initializerExpressionIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeStructureStatement(uint32 statementIndex) {
		const ref<StructureStatement> statement = std::static_pointer_cast<StructureStatement>(ast->statements[statementIndex]);
		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << indent() << "struct " << getTokenString(identifierToken) << " {\n";

		_indent += 1;

		for (const uint32 memberStatementIndex : statement->memberStatementIndices) {
			writeMemberStatement(memberStatementIndex);
		}

		_indent -= 1;

		stream << "};";
	}

	void GLSLCodeGenerator::writeMemberStatement(uint32 statementIndex) {
		const ref<StructureMemberStatement> statement = std::static_pointer_cast<StructureMemberStatement>(ast->statements[statementIndex]);

		if (statement->annotationFlag & (Annotation::INSTANCE_INDEX | Annotation::POSITION | Annotation::VERTEX_INDEX)) {
			return;
		}

		stream << indent();

		writeAnnotations(statement->annotationInstanceIndexOffset, statement->annotationInstanceCount);

		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << ";\n";
	}

	void GLSLCodeGenerator::writeFunctionStatement(uint32 statementIndex) {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(ast->statements[statementIndex]);

		stream << indent();

		writeTypeExpression(statement->returnTypeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << "(";

		for (uint32 parameterStatementIndexOffset = 0; parameterStatementIndexOffset < statement->parameterStatementIndices.size(); parameterStatementIndexOffset += 1) {
			const uint32 parameterStatementIndex = statement->parameterStatementIndices[parameterStatementIndexOffset];

			writeParameterStatement(parameterStatementIndex);

			if (parameterStatementIndexOffset + 1 < statement->parameterStatementIndices.size()) {
				stream << ", ";
			}
		}

		stream << ") ";

		writeScopeStatement(statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeVariableStatement(uint32 statementIndex) {
		const ref<VariableStatement> statement = std::static_pointer_cast<VariableStatement>(ast->statements[statementIndex]);

		stream << indent();

		if (statement->constant) {
			stream << "const ";
		}

		writeTypeExpression(statement->typeExpressionIndex);

		stream << " ";

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << identifierToken.value;

		if (statement->initializerExpressionIndex != ~0) {
			stream << " = ";

			writeExpression(statement->initializerExpressionIndex);
		}

		stream << ";";
	}

	void GLSLCodeGenerator::writeParameterStatement(uint32 statementIndex) {
		const ref<ParameterStatement> statement = std::static_pointer_cast<ParameterStatement>(ast->statements[statementIndex]);

		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken);
	}

	void GLSLCodeGenerator::writeScopeStatement(uint32 statementIndex) {
		const ref<ScopeStatement> statement = std::static_pointer_cast<ScopeStatement>(ast->statements[statementIndex]);

		stream << "{\n";

		_indent += 1;

		for (const uint32 statementIndex : statement->statementIndices) {
			writeStatement(statementIndex);

			stream << "\n";
		}

		_indent -= 1;

		stream << indent() << "}";
	}

	void GLSLCodeGenerator::writeIfStatement(uint32 statementIndex) {
		const ref<IfStatement> statement = std::static_pointer_cast<IfStatement>(ast->statements[statementIndex]);

		stream << indent() << "if (";

		writeExpression(statement->conditionExpressionIndex);

		stream << ") ";

		writeScopeStatement(statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeWhileStatement(uint32 statementIndex) {
		const ref<WhileStatement> statement = std::static_pointer_cast<WhileStatement>(ast->statements[statementIndex]);

		stream << indent() << "while (";

		writeExpression(statement->conditionExpressionIndex);

		stream << ") ";

		writeScopeStatement(statement->scopeStatementIndex);
	}

	void GLSLCodeGenerator::writeBreakStatement() {
		stream << indent() << "break;";
	}

	void GLSLCodeGenerator::writeNextStatement() {
		stream << indent() << "continue;";
	}

	void GLSLCodeGenerator::writeReturnStatement(uint32 statementIndex) {
		const ref<ReturnStatement> statement = std::static_pointer_cast<ReturnStatement>(ast->statements[statementIndex]);

		stream << indent() << "return ";

		writeExpression(statement->expressionIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeExpressionStatement(uint32 statementIndex) {
		const ref<ExpressionStatement> statement = std::static_pointer_cast<ExpressionStatement>(ast->statements[statementIndex]);

		stream << indent();
		
		writeExpression(statement->expressionIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeExpression(uint32 expressionIndex) {
		const ref<Expression>& expression = ast->expressions[expressionIndex];

		switch (expression->type) {
			case ExpressionType::NUMBER:
				return writeNumberExpression(expressionIndex);
			case ExpressionType::IDENTIFIER:
				return writeIdentifierExpression(expressionIndex);
			case ExpressionType::UNARY:
				return writeUnaryExpression(expressionIndex);
			case ExpressionType::BINARY:
				return writeBinaryExpression(expressionIndex);
			case ExpressionType::MEMBER_ACCESS:
				return writeMemberAccessExpression(expressionIndex);
			default:
				// TODO
				BREAKPOINT();

				break;
		}
	}

	void GLSLCodeGenerator::writeIdentifierExpression(uint32 expressionIndex) {
		const ref<IdentifierExpression> expression = std::static_pointer_cast<IdentifierExpression>(ast->expressions[expressionIndex]);
		const Token& identifierToken = result->tokens[expression->identifierTokenIndex];

		stream << identifierToken.value;
	}

	void GLSLCodeGenerator::writeNumberExpression(uint32 expressionIndex) {
		const ref<NumberExpression> expression = std::static_pointer_cast<NumberExpression>(ast->expressions[expressionIndex]);
		const Token& numberToken = result->tokens[expression->numberTokenIndex];

		// TODO
		ENGINE_ASSERT(
			numberToken.type != TokenType::LITERAL_INTEGER_BASE_2 &&
			numberToken.type != TokenType::LITERAL_INTEGER_BASE_8 &&
			numberToken.type != TokenType::LITERAL_INTEGER_BASE_16,
			"Bases 2, 8 and 16 are not supported yet."
		);

		stream << numberToken.value;
	}

	void GLSLCodeGenerator::writeUnaryExpression(uint32 expressionIndex) {
		const ref<UnaryExpression> expression = std::static_pointer_cast<UnaryExpression>(ast->expressions[expressionIndex]);
		const Token& operatorToken = result->tokens[expression->operatorTokenIndex];

		stream << getOperatorTokenString(operatorToken);

		writeExpression(expression->rightExpressionIndex);
	}

	void GLSLCodeGenerator::writeBinaryExpression(uint32 expressionIndex) {
		const ref<BinaryExpression> expression = std::static_pointer_cast<BinaryExpression>(ast->expressions[expressionIndex]);
		const Token& operatorToken = result->tokens[expression->operatorTokenIndex];

		writeExpression(expression->leftExpressionIndex);

		stream << " " << getOperatorTokenString(operatorToken) << " ";

		writeExpression(expression->rightExpressionIndex);
	}

	void GLSLCodeGenerator::writeMemberAccessExpression(uint32 expressionIndex) {
		const ref<MemberAccessExpression> expression = std::static_pointer_cast<MemberAccessExpression>(ast->expressions[expressionIndex]);

		writeExpression(expression->leftExpressionIndex);

		const Token& identifierToken = result->tokens[expression->identifierTokenIndex];

		stream << "." << identifierToken.value;
	}

	void GLSLCodeGenerator::writeTypeExpression(uint32 expressionIndex) {
		const ref<TypeExpression> expression = std::static_pointer_cast<TypeExpression>(ast->expressions[expressionIndex]);

		stream << getGlslType(expression->typeTokenIndex, expression->templateArgumentTokenIndex);
	}

	void GLSLCodeGenerator::writeAnnotations(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount) {
		if (annotationInstanceCount == 0) {
			return;
		}

		stream << "layout(";

		for (uint32 annotationInstanceIndex = 0; annotationInstanceIndex < annotationInstanceCount; annotationInstanceIndex += 1) {
			const AnnotationInstance& annotationInstance = ast->annotationInstances[annotationInstanceIndexOffset + annotationInstanceIndex];

			switch (annotationInstance.annotation) {
				case Annotation::LOCATION: {
					const Token& valueToken = result->tokens[annotationInstance.valueTokenIndex];

					stream << "location = " << getTokenString(valueToken) << "";

					break;
				}
				default:
					// TODO
					break;
			}

			if (annotationInstanceIndex + 1 < annotationInstanceCount) {
				stream << ", ";
			}
		}

		stream << ") ";
	}
}