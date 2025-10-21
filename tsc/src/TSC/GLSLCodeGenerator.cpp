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
			stream << "\n\n";

			writeStatement(globalScopeStatementIndex);
		}

		stream.close();
	}

	void GLSLCodeGenerator::writeStatement(uint32 globalScopeStatementIndex) {
		const ref<Statement> statement = ast->statements[globalScopeStatementIndex];

		switch (statement->statementType) {
			case StatementType::GLOBAL_SCOPE_CONSTANT:
				return writeGlobalScopeConstant(std::static_pointer_cast<GlobalScopeConstantStatement>(statement));
			case StatementType::STRUCTURE:
				return writeStructureStatement(std::static_pointer_cast<StructureStatement>(statement));
			case StatementType::FUNCTION:
				return writeFunctionStatement(std::static_pointer_cast<FunctionStatement>(statement));
			default:
				// TODO
				break;
		}
	}

	void GLSLCodeGenerator::writeGlobalScopeConstant(const ref<GlobalScopeConstantStatement> statement) {
		writeAnnotations(statement->annotationInstanceIndexOffset, statement->annotationInstanceCount);

		stream << "const ";

		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << " = ";

		writeExpression(statement->initializerExpressionIndex);

		stream << ";";
	}

	void GLSLCodeGenerator::writeStructureStatement(const ref<StructureStatement> statement) {
		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << indent() << "struct " << getTokenString(identifierToken) << " {\n";

		_indent += 1;

		for (const uint32 memberStatementIndex : statement->memberStatementIndices) {
			const ref<StructureMemberStatement> memberStatement = std::static_pointer_cast<StructureMemberStatement>(ast->statements[memberStatementIndex]);

			writeMemberStatement(memberStatement);
		}

		_indent -= 1;

		stream << "};";
	}

	void GLSLCodeGenerator::writeMemberStatement(const ref<StructureMemberStatement> statement) {
		if (statement->annotationFlag & (Annotation::INSTANCE_INDEX | Annotation::POSITION | Annotation::VERTEX_INDEX)) {
			return;
		}

		stream << indent();

		writeAnnotations(statement->annotationInstanceIndexOffset, statement->annotationInstanceCount);

		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << ";\n";
	}

	void GLSLCodeGenerator::writeFunctionStatement(const ref<FunctionStatement> statement) {
		stream << indent();

		writeTypeExpression(statement->returnTypeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken) << "(";

		for (uint32 parameterStatementIndexOffset = 0; parameterStatementIndexOffset < statement->parameterStatementIndices.size(); parameterStatementIndexOffset += 1) {
			const uint32 parameterStatementIndex = statement->parameterStatementIndices[parameterStatementIndexOffset];

			const ref<ParameterStatement> parameterStatement = std::static_pointer_cast<ParameterStatement>(ast->statements[parameterStatementIndex]);

			writeParameterStatement(parameterStatement);

			if (parameterStatementIndexOffset + 1 < statement->parameterStatementIndices.size()) {
				stream << ", ";
			}
		}

		stream << ") {\n";

		_indent += 1;

		stream << indent() << "//\n";

		_indent -= 1;

		stream << "}";
	}

	void GLSLCodeGenerator::writeParameterStatement(const ref<ParameterStatement> statement) {
		writeTypeExpression(statement->typeExpressionIndex);

		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << " " << getTokenString(identifierToken);
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
			default:
				// TODO
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
					stream << "location = " << annotationInstance.value.value << "";

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