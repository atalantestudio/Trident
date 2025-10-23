#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

#include "TSC/CompilationOutput.hpp"
#include "TSC/GLSLCodeGenerator.hpp"
#include "TSC/Lexer.hpp"
#include "TSC/Parser.hpp"

namespace ProjectA {
	std::string getIndent(uint32 indent) {
		return std::string(indent, '\t');
	}

	void writeAnnotations(std::ofstream& stream, uint32& indent, const CompilationOutput* output, uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount) {
		stream << "[\n";

		indent += 1;

		for (uint32 annotationInstanceIndex = 0; annotationInstanceIndex < annotationInstanceCount; annotationInstanceIndex += 1) {
			const AnnotationInstance& annotationInstance = output->ast.annotationInstances[annotationInstanceIndexOffset + annotationInstanceIndex];

			stream << getIndent(indent) << "{\n";

			indent += 1;

			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			stream << getIndent(indent) << "\"Annotation\": \"" << getTokenString(identifierToken) << "\"";

			if (annotationInstance.valueTokenIndex != ~0) {
				const Token& valueTokenIndex = output->tokens[annotationInstance.valueTokenIndex];

				stream << ",\n";
				stream << getIndent(indent) << "\"Value\": \"" << getTokenString(valueTokenIndex) << "\"";
			}

			stream << "\n";

			indent -= 1;

			stream << getIndent(indent) << "}";

			if (annotationInstanceIndex + 1 < annotationInstanceCount) {
				stream << ",";
			}

			stream << "\n";
		}

		indent -= 1;

		stream << getIndent(indent) << "]";
	}

	void writeExpression(std::ofstream& stream, uint32& indent, const CompilationOutput* output, uint32 expressionIndex) {
		const ref<Expression>& expression = output->ast.expressions[expressionIndex];

		stream << "{\n";

		indent += 1;

		switch (expression->type) {
			case ExpressionType::NUMBER: {
				const ref<NumberExpression> numberExpression = std::static_pointer_cast<NumberExpression>(expression);
				const Token& numberToken = output->tokens[numberExpression->numberTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"NumberExpression\",\n";
				stream << getIndent(indent) << "\"Number\": \"" << getTokenString(numberToken) << "\"";

				break;
			}
			case ExpressionType::IDENTIFIER: {
				const ref<IdentifierExpression> identifierExpression = std::static_pointer_cast<IdentifierExpression>(expression);
				const Token& identifierToken = output->tokens[identifierExpression->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"IdentifierExpression\",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";

				break;
			}
			case ExpressionType::UNARY: {
				const ref<UnaryExpression> unaryExpression = std::static_pointer_cast<UnaryExpression>(expression);
				const Token& operatorToken = output->tokens[unaryExpression->operatorTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"UnaryExpression\",\n";
				stream << getIndent(indent) << "\"Operator\": \"" << getTokenString(operatorToken) << ",\n";
				stream << getIndent(indent) << "\"Right\": ";

				writeExpression(stream, indent, output, unaryExpression->rightExpressionIndex);

				break;
			}
			case ExpressionType::BINARY: {
				const ref<BinaryExpression> binaryExpression = std::static_pointer_cast<BinaryExpression>(expression);
				const Token& operatorToken = output->tokens[binaryExpression->operatorTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"BinaryExpression\",\n";
				stream << getIndent(indent) << "\"Left\": ";

				writeExpression(stream, indent, output, binaryExpression->leftExpressionIndex);

				stream << ",\n";

				stream << getIndent(indent) << "\"Operator\": \"" << getTokenString(operatorToken) << "\",\n";
				stream << getIndent(indent) << "\"Right\": ";

				writeExpression(stream, indent, output, binaryExpression->rightExpressionIndex);

				break;
			}
			case ExpressionType::GROUP: {
				const ref<GroupExpression> groupExpression = std::static_pointer_cast<GroupExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"GroupExpression\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, output, groupExpression->expressionIndex);

				break;
			}
			case ExpressionType::MEMBER_ACCESS: {
				const ref<MemberAccessExpression> memberAccessExpression = std::static_pointer_cast<MemberAccessExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"MemberAccessExpression\",\n";
				stream << getIndent(indent) << "\"Left\": ";

				writeExpression(stream, indent, output, memberAccessExpression->leftExpressionIndex);

				const Token& identifierTokenIndex = output->tokens[memberAccessExpression->identifierTokenIndex];

				stream << getIndent(indent) << "\"MemberIdentifier\": \"" << getTokenString(identifierTokenIndex) << "\"";

				break;
			}
			case ExpressionType::FUNCTION_CALL: {
				const ref<FunctionCallExpression> functionCallExpression = std::static_pointer_cast<FunctionCallExpression>(expression);

				stream << getIndent(indent) << "\"Type\": \"FunctionCallExpression\",\n";
				stream << getIndent(indent) << "\"Callee\": ";

				writeExpression(stream, indent, output, functionCallExpression->calleeExpressionIndex);

				stream << getIndent(indent) << "\"Arguments\": [\n";

				indent += 1;

				for (const uint32 argumentExpressionIndex : functionCallExpression->argumentExpressionIndices) {
					stream << getIndent(indent);

					writeExpression(stream, indent, output, argumentExpressionIndex);
				}

				indent -= 1;

				stream << getIndent(indent) << "]";

				break;
			}
			case ExpressionType::TYPE: {
				const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(expression);
				const Token& typeToken = output->tokens[typeExpression->typeTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"TypeExpression\",\n";
				stream << getIndent(indent) << "\"_Type\": \"" << getTokenString(typeToken) << "\"";

				if (typeExpression->templateArgumentTokenIndex != ~0) {
					const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

					stream << ",\n";
					stream << getIndent(indent) << "\"TemplateArgument\": \"" << getTokenString(templateArgumentToken) << "\"";
				}

				break;
			}
			default:
				// TODO
				BREAKPOINT();

				break;
		}

		indent -= 1;

		stream << "\n" << getIndent(indent) << "}";
	}

	void writeStatement(std::ofstream& stream, uint32& indent, const CompilationOutput* output, uint32 statementIndex) {
		const ref<Statement>& statement = output->ast.statements[statementIndex];

		stream << "{\n";

		indent += 1;

		switch (statement->statementType) {
			case StatementType::STRUCTURE: {
				const ref<StructureStatement> structureStatement = std::static_pointer_cast<StructureStatement>(statement);
				const Token& identifierToken = output->tokens[structureStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"StructureStatement\",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Members\": [\n";

				indent += 1;

				for (uint32 memberStatementIndexOffset = 0; memberStatementIndexOffset < structureStatement->memberStatementIndices.size(); memberStatementIndexOffset += 1) {
					const uint32 memberStatementIndex = structureStatement->memberStatementIndices[memberStatementIndexOffset];

					stream << getIndent(indent);

					writeStatement(stream, indent, output, memberStatementIndex);

					if (memberStatementIndexOffset + 1 < structureStatement->memberStatementIndices.size()) {
						stream << ",";
					}

					stream << "\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "]";

				break;
			}
			case StatementType::STRUCTURE_MEMBER: {
				const ref<StructureMemberStatement> structureMemberStatement = std::static_pointer_cast<StructureMemberStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"StructureMemberStatement\",\n";
				stream << getIndent(indent) << "\"AnnotationFlag\": " << structureMemberStatement->annotationFlag << ",\n";
				stream << getIndent(indent) << "\"Annotations\": ";

				writeAnnotations(stream, indent, output, structureMemberStatement->annotationInstanceIndexOffset, structureMemberStatement->annotationInstanceCount);

				stream << ",\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, output, structureMemberStatement->typeExpressionIndex);

				const Token& identifierToken = output->tokens[structureMemberStatement->identifierTokenIndex];

				stream << ",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";

				break;
			}
			case StatementType::GLOBAL_SCOPE_CONSTANT: {
				const ref<GlobalScopeConstantStatement> constantStatement = std::static_pointer_cast<GlobalScopeConstantStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"GlobalScopeConstantStatement\",\n";
				stream << getIndent(indent) << "\"AnnotationFlag\": " << constantStatement->annotationFlag << ",\n";
				stream << getIndent(indent) << "\"Annotations\": ";

				writeAnnotations(stream, indent, output, constantStatement->annotationInstanceIndexOffset, constantStatement->annotationInstanceCount);

				stream << ",\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, output, constantStatement->typeExpressionIndex);

				const Token& identifierToken = output->tokens[constantStatement->identifierTokenIndex];

				stream << ",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Initializer\": ";

				writeExpression(stream, indent, output, constantStatement->initializerExpressionIndex);

				break;
			}
			case StatementType::VARIABLE: {
				const ref<VariableStatement> variableStatement = std::static_pointer_cast<VariableStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"VariableStatement\",\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, output, variableStatement->typeExpressionIndex);

				const Token& identifierToken = output->tokens[variableStatement->identifierTokenIndex];

				stream << ",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"IsConstant\": \"" << std::to_string(variableStatement->constant) << "\",\n";
				stream << getIndent(indent) << "\"Initializer\": ";

				if (variableStatement->initializerExpressionIndex != ~0) {
					writeExpression(stream, indent, output, variableStatement->initializerExpressionIndex);
				}
				else {
					stream << "null";
				}

				break;
			}
			case StatementType::FUNCTION: {
				const ref<FunctionStatement> functionStatement = std::static_pointer_cast<FunctionStatement>(statement);
				const Token& identifierToken = output->tokens[functionStatement->identifierTokenIndex];

				stream << getIndent(indent) << "\"Type\": \"FunctionStatement\",\n";
				stream << getIndent(indent) << "\"ReturnType\": ";

				writeExpression(stream, indent, output, functionStatement->returnTypeExpressionIndex);

				stream << ",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\",\n";
				stream << getIndent(indent) << "\"Arguments\": [\n";

				indent += 1;

				for (uint32 parameterStatementIndexOffset = 0; parameterStatementIndexOffset < functionStatement->parameterStatementIndices.size(); parameterStatementIndexOffset += 1) {
					const uint32 parameterStatementIndex = functionStatement->parameterStatementIndices[parameterStatementIndexOffset];

					stream << getIndent(indent);

					writeStatement(stream, indent, output, parameterStatementIndex);

					if (parameterStatementIndexOffset + 1 < functionStatement->parameterStatementIndices.size()) {
						stream << ",";
					}

					stream << "\n";
				}

				indent -= 1;

				stream << getIndent(indent) << "],\n";
				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, output, functionStatement->scopeStatementIndex);

				break;
			}
			case StatementType::PARAMETER: {
				const ref<ParameterStatement> parameterStatement = std::static_pointer_cast<ParameterStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ParameterStatement\",\n";
				stream << getIndent(indent) << "\"_Type\": ";

				writeExpression(stream, indent, output, parameterStatement->typeExpressionIndex);

				const Token& identifierToken = output->tokens[parameterStatement->identifierTokenIndex];

				stream << ",\n";
				stream << getIndent(indent) << "\"Identifier\": \"" << getTokenString(identifierToken) << "\"";

				break;
			}
			case StatementType::EXPRESSION: {
				const ref<ExpressionStatement> expressionStatement = std::static_pointer_cast<ExpressionStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ExpressionStatement\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, output, expressionStatement->expressionIndex);

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

					writeStatement(stream, indent, output, statementIndex);

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

				writeExpression(stream, indent, output, ifStatement->conditionExpressionIndex);

				stream << ",\n";
				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, output, ifStatement->scopeStatementIndex);

				break;
			}
			case StatementType::WHILE: {
				const ref<WhileStatement> whileStatement = std::static_pointer_cast<WhileStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"WhileStatement\",\n";
				stream << getIndent(indent) << "\"Condition\": ";

				writeExpression(stream, indent, output, whileStatement->conditionExpressionIndex);

				stream << ",\n";
				stream << getIndent(indent) << "\"Scope\": ";

				writeStatement(stream, indent, output, whileStatement->scopeStatementIndex);

				break;
			}
			case StatementType::BREAK: {
				stream << getIndent(indent) << "\"Type\": \"BreakStatement\"";

				break;
			}
			case StatementType::NEXT: {
				stream << getIndent(indent) << "\"Type\": \"NextStatement\"";

				break;
			}
			case StatementType::RETURN: {
				const ref<ReturnStatement> returnStatement = std::static_pointer_cast<ReturnStatement>(statement);

				stream << getIndent(indent) << "\"Type\": \"ReturnStatement\",\n";
				stream << getIndent(indent) << "\"Expression\": ";

				writeExpression(stream, indent, output, returnStatement->expressionIndex);

				break;
			}
			default:
				// TODO
				BREAKPOINT();

				break;
		}

		indent -= 1;

		stream << "\n" << getIndent(indent) << "}";
	}

	static sint32 tsc(const char8* path) {
		const sequence<uint8> buffer = Filesystem::openBuffer("Test/test0.tsl");
		// const sequence<uint8> buffer = Filesystem::openBuffer("Test/diagnostic.tsl");
		const std::string text(buffer.begin(), buffer.end());

		CompilationOutput output{};
		output.text = text;

		Lexer lexer;
		lexer.tokenize(&output);

		Parser parser;
		parser.parse(&output);

		bool hasError = false;

		for (const Diagnostic& diagnostic : output.diagnostics) {
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

		GLSLCodeGenerator generator;
		generator.write("Test/test0.glsl", &output);

		#if 1
			std::ofstream stream("Test/test0.ast.json");

			DEBUG_ASSERT(stream.is_open());

			uint32 indent = 0;

			stream << "[\n";

			indent += 1;

			for (uint32 globalScopeStatementIndexOffset = 0; globalScopeStatementIndexOffset < output.ast.globalScopeStatementIndices.size(); globalScopeStatementIndexOffset += 1) {
				const uint32 globalScopeStatementIndex = output.ast.globalScopeStatementIndices[globalScopeStatementIndexOffset];

				stream << getIndent(indent);

				writeStatement(stream, indent, &output, globalScopeStatementIndex);

				if (globalScopeStatementIndexOffset + 1 < output.ast.globalScopeStatementIndices.size()) {
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