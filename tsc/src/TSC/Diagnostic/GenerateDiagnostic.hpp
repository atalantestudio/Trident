#pragma once

#include "Diagnostic.hpp"
#include "../Annotation.hpp"
#include "../Statement.hpp"
#include "../Token.hpp"

namespace ProjectA {
	// TODO: Make private in CompilationOutput.
	template<DiagnosticCode DiagnosticCode>
	struct GenerateDiagnostic;

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER> {
		static void prepare(Diagnostic& diagnostic, char8 character, DocumentPosition start) {
			diagnostic.range.start = start;
			diagnostic.range.end.lineIndex = start.lineIndex;
			diagnostic.range.end.characterOffset = 1;
			diagnostic.message = MODULE_FORMAT("Unexpected character \"[]\".", character);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::MISSING_INCLUDE_PATH> {
		static void prepare(Diagnostic& diagnostic, DocumentPosition start, DocumentPosition end) {
			diagnostic.range.start = start;
			diagnostic.range.end = end;
			diagnostic.message = "Missing include path.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INCLUDE_FILE_NOT_FOUND> {
		static void prepare(Diagnostic& diagnostic, const IncludeStatement& includeStatement) {
			diagnostic.range = includeStatement.range;
			diagnostic.message = MODULE_FORMAT("Include file not found at \"[]\".", includeStatement.path);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::NUMBER_OVERFLOW> {
		static void prepare(Diagnostic& diagnostic, const Token& token) {
			diagnostic.range = token.getRange();
			diagnostic.message = "Number exceeds the range of a 64-bit unsigned integer.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_NUMBER_LITERAL> {
		static void prepare(Diagnostic& diagnostic, const Token& token) {
			diagnostic.range.start = token.start;

			// The token could not be finished.
			diagnostic.range.end = token.start;

			diagnostic.message = "Invalid number literal.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNEXPECTED_TOKEN> {
		static void prepare(Diagnostic& diagnostic, const std::string& tokenString, const Token& unexpectedToken) {
			diagnostic.range = unexpectedToken.getRange();
			diagnostic.message = MODULE_FORMAT("Expected '[]'.", tokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::MISSING_EXPRESSION> {
		static void prepare(Diagnostic& diagnostic, const Token& token) {
			diagnostic.range = token.getRange();
			diagnostic.message = "Expected an expression.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::MISSING_IDENTIFIER> {
		static void prepare(Diagnostic& diagnostic, const Token& token) {
			diagnostic.range = token.getRange();
			diagnostic.message = "Expected an identifier.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::REDEFINITION> {
		static void prepare(Diagnostic& diagnostic, const Token& redefinitionToken, const std::string& redefinitionTokenString, const Token& definitionToken, const std::string& definitionTokenString) {
			DiagnosticReference diagnosticReference{};
			diagnosticReference.range = definitionToken.getRange();
			diagnosticReference.message = MODULE_FORMAT("\"[]\" is defined here.", definitionTokenString);

			diagnostic.range = redefinitionToken.getRange();
			diagnostic.message = MODULE_FORMAT("Redefinition of \"[]\".", redefinitionTokenString);
			diagnostic.references.emplace_back(diagnosticReference);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT> {
		static void prepare(Diagnostic& diagnostic, const Token& typeToken, const std::string& typeTokenString) {
			diagnostic.range = typeToken.getRange();
			diagnostic.message = MODULE_FORMAT("Missing template argument for type \"[]\".", typeTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT> {
		static void prepare(Diagnostic& diagnostic, const std::string& typeTokenString, const Token& templateArgumentToken, const std::string& templateArgumentTokenString) {
			diagnostic.range = templateArgumentToken.getRange();
			diagnostic.message = MODULE_FORMAT("\"[]\" cannot be used as a template argument for type \"[]\".", templateArgumentTokenString, typeTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNDEFINED_TYPE> {
		static void prepare(Diagnostic& diagnostic, const Token& typeToken, const std::string& typeTokenString) {
			diagnostic.range = typeToken.getRange();
			diagnostic.message = MODULE_FORMAT("Undefined type \"[]\".", typeTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE> {
		static void prepare(Diagnostic& diagnostic, const Token& typeToken, const std::string& typeTokenString) {
			diagnostic.range = typeToken.getRange();
			diagnostic.message = MODULE_FORMAT("Incomplete type \"[]\".", typeTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::DUPLICATE_ANNOTATION> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Duplicate annotation \"[]\".", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Missing value for annotation \"[]\".", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Undefined identifier \"[]\".", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNDEFINED_FUNCTION> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Undefined function \"[]\".", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNDEFINED_FUNCTION_OVERLOAD> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Cannot find overload for function \"[]\".", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::EXPRESSION_NOT_CONSTANT> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& range) {
			diagnostic.range = range;
			diagnostic.message = "Expression is not constant.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNDEFINED_ANNOTATION> {
		static void prepare(Diagnostic& diagnostic, const Token& token, const std::string& tokenString) {
			diagnostic.range = token.getRange();
			diagnostic.message = MODULE_FORMAT("Undefined annotation \"[]\".", tokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_CONVERSION> {
		static void prepare(Diagnostic& diagnostic, const Token& token) {
			diagnostic.range = token.getRange();
			diagnostic.message = "Invalid conversion.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT> {
		static void prepare(Diagnostic& diagnostic, const Token& literalToken, const std::string& typeString) {
			diagnostic.range = literalToken.getRange();
			diagnostic.message = MODULE_FORMAT("Floating-point number literal requires explicit cast in order to be assigned to type [].", typeString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_ANNOTATION_TYPE_COMBINATION> {
		static void prepare(Diagnostic& diagnostic, Annotation annotation, const Token& identifierToken, const std::string& typeString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Annotation \"[]\" cannot be used on a member of type [].", ANNOTATION_STRINGS[annotation], typeString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::ARGUMENT_COUNT_MISMATCH> {
		static void prepare(Diagnostic& diagnostic, const std::string& typeString, uint32 expectedArgumentCount, uint32 argumentCount) {
			// TODO: Highlight the correct argument range.
			diagnostic.range.start = {0, 0};
			diagnostic.range.end = {0, 0};

			diagnostic.message = MODULE_FORMAT("[] constructor requires [] argument(s) but was provided with [].", typeString, expectedArgumentCount, argumentCount);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Structure is empty.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::NO_RETURN> {
		static void prepare(Diagnostic& diagnostic, const Token& identifierToken, const std::string& identifierTokenString) {
			diagnostic.range = identifierToken.getRange();
			diagnostic.message = MODULE_FORMAT("Function \"[]\" doesn't end with a return statement.", identifierTokenString);
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::CONSTANT_ASSIGNMENT> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& operatorRange) {
			diagnostic.range = operatorRange;
			diagnostic.message = "Cannot assign to a constant post-initialization.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use PascalCase for structure identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use camelCase or snake_case for member identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use SCREAMING_SNAKE_CASE for constant identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use camelCase or snake_case for function identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use camelCase or snake_case for function parameter identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.range = identifierRange;
			diagnostic.message = "Use camelCase or snake_case for variable identifiers.";
		}
	};

	template<>
	struct GenerateDiagnostic<DiagnosticCode::UNUSED_OBJECT> {
		static void prepare(Diagnostic& diagnostic, const DocumentRange& identifierRange) {
			diagnostic.severity = DiagnosticSeverity::HINT_OR_WIP;
			diagnostic.range = identifierRange;
			diagnostic.message = "Object is unused.";
		}
	};
}