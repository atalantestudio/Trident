#pragma once

#include "../CompilationOutput.hpp"
#include "Diagnostic.hpp"
#include "DiagnosticCode.hpp"
#include "DiagnosticSeverity.hpp"
#include "generateDiagnostic.hpp"
#include "../Statement.hpp"
#include "../Token.hpp"

namespace ProjectA {
	template<>
	struct generateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER> {
		explicit generateDiagnostic(CompilationOutput& output, char8 character, DocumentPosition start) {
			Diagnostic diagnostic{};
			diagnostic.start = start;
			diagnostic.end.lineIndex = start.lineIndex;
			diagnostic.end.characterOffset = 1;
			diagnostic.message = MODULE_FORMAT("Unexpected character \"[]\".", character);
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::UNEXPECTED_CHARACTER;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::MISSING_INCLUDE_PATH> {
		explicit generateDiagnostic(CompilationOutput& output, DocumentPosition start, DocumentPosition end) {
			Diagnostic diagnostic{};
			diagnostic.start = start;
			diagnostic.end = end;
			diagnostic.message = "Missing include path.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::MISSING_INCLUDE_PATH;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INCLUDE_FILE_NOT_FOUND> {
		explicit generateDiagnostic(CompilationOutput& output, const IncludeStatement& includeStatement) {
			Diagnostic diagnostic{};
			diagnostic.start = includeStatement.start;
			diagnostic.end = includeStatement.end;
			diagnostic.message = MODULE_FORMAT("Include file not found at \"[]\".", includeStatement.path);
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INCLUDE_FILE_NOT_FOUND;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::MISSING_EXPRESSION> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& token) {
			Diagnostic diagnostic{};
			diagnostic.start = token.start;
			diagnostic.end = token.getEnd();
			diagnostic.message = "Expected an expression.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::MISSING_EXPRESSION;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::MISSING_IDENTIFIER> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& token) {
			Diagnostic diagnostic{};
			diagnostic.start = token.start;
			diagnostic.end = token.getEnd();
			diagnostic.message = "Expected an identifier.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::MISSING_IDENTIFIER;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::REDEFINITION> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& definitionToken, const Token& redefinitionToken) {
			DiagnosticReference diagnosticReference{};
			diagnosticReference.start = definitionToken.start;
			diagnosticReference.end = definitionToken.getEnd();
			diagnosticReference.message = MODULE_FORMAT("\"[]\" is defined here.", output.getTokenString(definitionToken));

			Diagnostic diagnostic{};
			diagnostic.start = redefinitionToken.start;
			diagnostic.end = redefinitionToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Redefinition of \"[]\".", output.getTokenString(redefinitionToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::REDEFINITION;
			diagnostic.references.emplace_back(diagnosticReference);

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT> {
		explicit generateDiagnostic(CompilationOutput& output, uint32 typeTokenIndex) {
			const Token& typeToken = output.tokens[typeTokenIndex];

			Diagnostic diagnostic{};
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Missing template argument for type \"[]\".", output.getTokenString(typeToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::MISSING_TEMPLATE_ARGUMENT;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT> {
		explicit generateDiagnostic(CompilationOutput& output, uint32 typeTokenIndex, uint32 templateArgumentTokenIndex) {
			const Token& typeToken = output.tokens[typeTokenIndex];
			const Token& templateArgumentToken = output.tokens[templateArgumentTokenIndex];

			Diagnostic diagnostic{};
			diagnostic.start = templateArgumentToken.start;
			diagnostic.end = templateArgumentToken.getEnd();
			diagnostic.message = MODULE_FORMAT("\"[]\" cannot be used as a template argument for type \"[]\".", output.getTokenString(templateArgumentToken), output.getTokenString(typeToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE> {
		explicit generateDiagnostic(CompilationOutput& output, uint32 typeTokenIndex) {
			const Token& typeToken = output.tokens[typeTokenIndex];

			Diagnostic diagnostic{};
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Undefined type \"[]\".", output.getTokenString(typeToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::UNDEFINED_TYPE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& typeToken) {
			Diagnostic diagnostic{};
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Incomplete type \"[]\".", output.getTokenString(typeToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INCOMPLETE_TYPE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::DUPLICATE_ANNOTATION> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Duplicate annotation \"[]\".", output.getTokenString(identifierToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::DUPLICATE_ANNOTATION;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.getEnd();
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Missing value for annotation \"[]\".", output.getTokenString(identifierToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::MISSING_ANNOTATION_VALUE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Undefined identifier \"[]\".", output.getTokenString(identifierToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::UNDEFINED_IDENTIFIER;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use PascalCase for structure identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use camelCase or snake_case for member identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use SCREAMING_SNAKE_CASE for constant identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use camelCase or snake_case for function identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use camelCase or snake_case for function parameter identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Use camelCase or snake_case for variable identifiers.";
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::NO_RETURN> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = MODULE_FORMAT("Function \"[]\" doesn't end with a return statement.", output.getTokenString(identifierToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::NO_RETURN;

			output.diagnostics.emplace_back(diagnostic);
		}
	};
}