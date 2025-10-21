#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

#include "GLSLTranspiler.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
// #include "SemanticAnalyzer.hpp"

namespace ProjectA {
	std::string getIndent(uint32 indent) {
		return std::string(indent, '\t');
	}

	void writeExpression(std::ofstream& stream, uint32& indent, const CompilationResult& result, const AST& ast, uint32 expressionIndex) {
		const ref<Expression>& expression = ast.expressions[expressionIndex];

		stream << "{\n";

		indent += 1;

		switch (expression->type) {
			case ExpressionType::NUMBER: {
				const ref<NumberExpression> numberExpression = std::static_pointer_cast<NumberExpression>(expression);
				const Token& numberToken = result.tokens[numberExpression->numberTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"NumberExpression\",\n";
				stream << getIndent(indent) << "\"Number\": \"" << getTokenString(numberToken) << "\"";

				break;
			}
			case ExpressionType::IDENTIFIER: {
				const ref<IdentifierExpression> identifierExpression = std::static_pointer_cast<IdentifierExpression>(expression);
				const Token& identifierToken = result.tokens[identifierExpression->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"IdentifierExpression\",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken);

				break;
			}
			case ExpressionType::UNARY: {
				const ref<UnaryExpression> unaryExpression = std::static_pointer_cast<UnaryExpression>(expression);
				const Token& operatorToken = result.tokens[unaryExpression->operatorTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"UnaryExpression\",\n";
				stream << getIndent(indent) << "\"Operator\": \"" << getTokenString(operatorToken) << ",\n";
				stream << getIndent(indent) << "\"Right\": ";

				writeExpression(stream, indent, result, ast, unaryExpression->rightExpressionIndex);

				break;
			}
			case ExpressionType::BINARY: {
				const ref<BinaryExpression> binaryExpression = std::static_pointer_cast<BinaryExpression>(expression);
				const Token& operatorToken = result.tokens[binaryExpression->operatorTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"BinaryExpression\",\n";
				stream << getIndent(indent) << "\"Left\": ";

				writeExpression(stream, indent, result, ast, binaryExpression->leftExpressionIndex);

				stream << getIndent(indent) << "\"Operator\": \"" << getTokenString(operatorToken) << "\",\n";
				stream << getIndent(indent) << "\"Right\": ";

				writeExpression(stream, indent, result, ast, binaryExpression->rightExpressionIndex);

				break;
			}
			case ExpressionType::GROUP: {
				const ref<GroupExpression> groupExpression = std::static_pointer_cast<GroupExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"GroupExpression\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, result, ast, groupExpression->expressionIndex);

				break;
			}
			case ExpressionType::MEMBER_ACCESS: {
				const ref<MemberAccessExpression> memberAccessExpression = std::static_pointer_cast<MemberAccessExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"MemberAccessExpression\",\n";
				stream << getIndent(indent) << "\"Left\": ";

				writeExpression(stream, indent, result, ast, memberAccessExpression->leftExpressionIndex);

				stream << getIndent(indent) << "\"MemberIdentifier\": \"" << memberAccessExpression->memberIdentifier << "\"";

				break;
			}
			case ExpressionType::FUNCTION_CALL: {
				const ref<FunctionCallExpression> functionCallExpression = std::static_pointer_cast<FunctionCallExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"FunctionCallExpression\",\n";
				stream << getIndent(indent) << "\"Callee\": ";

				writeExpression(stream, indent, result, ast, functionCallExpression->calleeExpressionIndex);

				stream << getIndent(indent) << "\"Arguments\": [\n";

				indent += 1;

				for (const uint32 argumentExpressionIndex : functionCallExpression->argumentExpressionIndices) {
					stream << getIndent(indent);

					writeExpression(stream, indent, result, ast, argumentExpressionIndex);
				}

				indent -= 1;

				stream << getIndent(indent) << "]";

				break;
			}
			case ExpressionType::TYPE: {
				const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(expression);
				const Token& typeToken = result.tokens[typeExpression->typeTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"TypeExpression\",\n";
				stream << getIndent(indent) << "\"_Type\": \"" << getTokenString(typeToken) << "\"";

				if (typeExpression->templateArgumentTokenIndex != ~0) {
					const Token& templateArgumentToken = result.tokens[typeExpression->templateArgumentTokenIndex];

					stream << ",\n";
					stream << getIndent(indent) << "\"TemplateArgument\": \"" << getTokenString(templateArgumentToken) << "\"";
				}

				break;
			}
		}

		indent -= 1;

		stream << "\n" << getIndent(indent) << "},\n";
	}

	void writeStatement(std::ofstream& stream, uint32& indent, const CompilationResult& result, const AST& ast, uint32 statementIndex) {
		const ref<Statement>& statement = ast.statements[statementIndex];

		stream << "{\n";

		indent += 1;

		switch (statement->statementType) {
			case StatementType::STRUCTURE: {
				const ref<StructureStatement> structureStatement = std::static_pointer_cast<StructureStatement>(statement);
				const Token& identifierToken = result.tokens[structureStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"StructureStatement\",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Members\": [\n";

				indent += 1;

				for (uint32 memberStatementIndexOffset = 0; memberStatementIndexOffset < structureStatement->memberStatementIndices.size(); memberStatementIndexOffset += 1) {
					const uint32 memberStatementIndex = structureStatement->memberStatementIndices[memberStatementIndexOffset];

					stream << getIndent(indent);

					writeStatement(stream, indent, result, ast, memberStatementIndex);

					if (memberStatementIndexOffset + 1 < structureStatement->memberStatementIndices.size()) {
						stream << ",";
					}

					stream << "\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "]";

				break;
			}
			case StatementType::VARIABLE: {
				const ref<VariableStatement> variableStatement = std::static_pointer_cast<VariableStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"VariableStatement\",\n";
				stream << getIndent(indent) << "\"AnnotationFlag\": " << variableStatement->annotationFlag << ",\n";
				stream << getIndent(indent) << "\"Annotations\": [\n";

				indent += 1;

				for (uint32 annotationInstanceIndex = 0; annotationInstanceIndex < variableStatement->annotationInstanceCount; annotationInstanceIndex += 1) {
					const AnnotationInstance& annotationInstance = ast.annotationInstances[variableStatement->annotationInstanceIndexOffset + annotationInstanceIndex];

					stream << getIndent(indent) << "{\n";

					indent += 1;

					stream << getIndent(indent) << "\"Annotation\": \"" << getAnnotationString(annotationInstance.annotation) << "\",\n";

					if (!annotationInstance.value.value.empty()) {
						stream << getIndent(indent) << "\"Value\": \"" << getTokenString(annotationInstance.value) << "\",\n";
					}

					indent -= 1;

					stream << getIndent(indent) << "},\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "],\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, result, ast, variableStatement->typeExpressionIndex);

				const Token& identifierToken = result.tokens[variableStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Initializer\": ";

				if (variableStatement->initializerExpressionIndex != ~0) {
					writeExpression(stream, indent, result, ast, variableStatement->initializerExpressionIndex);
				}
				else {
					stream << "null";
				}

				break;
			}
			case StatementType::FUNCTION: {
				const ref<FunctionStatement> functionStatement = std::static_pointer_cast<FunctionStatement>(statement);
				const Token& identifierToken = result.tokens[functionStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"FunctionStatement\",\n";
				stream << getIndent(indent) << "\"ReturnType\": ";

				writeExpression(stream, indent, result, ast, functionStatement->returnTypeExpressionIndex);

				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Arguments\": [\n";

				indent += 1;

				for (uint32 parameterStatementIndexOffset = 0; parameterStatementIndexOffset < functionStatement->parameterStatementIndices.size(); parameterStatementIndexOffset += 1) {
					const uint32 parameterStatementIndex = functionStatement->parameterStatementIndices[parameterStatementIndexOffset];

					stream << getIndent(indent);

					writeStatement(stream, indent, result, ast, parameterStatementIndex);

					if (parameterStatementIndexOffset + 1 < functionStatement->parameterStatementIndices.size()) {
						stream << ",";
					}

					stream << "\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "],\n";
				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, result, ast, functionStatement->scopeStatementIndex);

				break;
			}
			case StatementType::PARAMETER: {
				const ref<ParameterStatement> parameterStatement = std::static_pointer_cast<ParameterStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ParameterStatement\",\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, result, ast, parameterStatement->typeExpressionIndex);

				const Token& identifierToken = result.tokens[parameterStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";

				break;
			}
			case StatementType::EXPRESSION: {
				const ref<ExpressionStatement> expressionStatement = std::static_pointer_cast<ExpressionStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ExpressionStatement\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, result, ast, expressionStatement->expressionIndex);

				break;
			}
			case StatementType::SCOPE: {
				const ref<ScopeStatement> scopeStatement = std::static_pointer_cast<ScopeStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ScopeStatement\",\n";
				stream << getIndent(indent) << "\"Statements\": [\n";

				indent += 1;

				for (uint32 statementIndexOffset = 0; statementIndexOffset < scopeStatement->statementIndices.size(); statementIndexOffset += 1) {
					const uint32 statementIndex = scopeStatement->statementIndices[statementIndexOffset];

					stream << getIndent(indent);

					writeStatement(stream, indent, result, ast, statementIndex);

					if (statementIndexOffset + 1 < scopeStatement->statementIndices.size()) {
						stream << ",";
					}

					stream << "\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "]";

				break;
			}
			case StatementType::IF: {
				const ref<IfStatement> ifStatement = std::static_pointer_cast<IfStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"IfStatement\",\n";
				stream << getIndent(indent) << "\"Condition\": ";

				writeExpression(stream, indent, result, ast, ifStatement->conditionExpressionIndex);

				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, result, ast, ifStatement->scopeStatementIndex);

				break;
			}
			case StatementType::WHILE: {
				const ref<WhileStatement> whileStatement = std::static_pointer_cast<WhileStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"WhileStatement\",\n";
				stream << getIndent(indent) << "\"Condition\": ";

				writeExpression(stream, indent, result, ast, whileStatement->conditionExpressionIndex);

				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, result, ast, whileStatement->scopeStatementIndex);

				break;
			}
			case StatementType::RETURN: {
				const ref<ReturnStatement> returnStatement = std::static_pointer_cast<ReturnStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ReturnStatement\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, result, ast, returnStatement->expressionIndex);

				break;
			}
			default:
				break;
		}

		indent -= 1;

		stream << "\n" << getIndent(indent) << "}";
	}

	static sint32 tsc(const char8* path) {
		const sequence<uint8> buffer = Filesystem::openBuffer("Test/test0.tsl");
		const std::string characters(buffer.begin(), buffer.end());

		CompilationResult result{};

		Lexer lexer;
		lexer.tokenize(characters, result);

		Parser parser;
		const AST ast = parser.parse(result);

		bool hasError = false;

		for (const Diagnostic& diagnostic : result.diagnostics) {
			switch (diagnostic.severity) {
				case DiagnosticSeverity::ERROR: {
					ENGINE_ERROR(diagnostic.message);

					hasError = true;

					break;
				}
				case DiagnosticSeverity::WARNING: {
					ENGINE_WARN(diagnostic.message);

					break;
				}
				case DiagnosticSeverity::INFO:
				case DiagnosticSeverity::HINT_OR_WIP: {
					ENGINE_INFO(diagnostic.message);

					break;
				}
			}
		}

		if (hasError) {
			return 1;
		}

		GLSLTranspiler transpiler;
		transpiler.write("Test/test0.glsl", result, ast);

		#if 1
			std::ofstream stream("Test/test0.ast.json");

			DEBUG_ASSERT(stream.is_open());

			uint32 indent = 0;

			stream << "[\n";

			indent += 1;

			for (uint32 globalScopeStatementIndexOffset = 0; globalScopeStatementIndexOffset < ast.globalScopeStatementIndices.size(); globalScopeStatementIndexOffset += 1) {
				const uint32 globalScopeStatementIndex = ast.globalScopeStatementIndices[globalScopeStatementIndexOffset];

				stream << getIndent(indent);

				writeStatement(stream, indent, result, ast, globalScopeStatementIndex);

				if (globalScopeStatementIndexOffset + 1 < ast.globalScopeStatementIndices.size()) {
					stream << ",";
				}

				stream << "\n";
			}

			indent -= 1;

			stream << "]";
			stream.close();
		#endif

		return 0;
	}
}