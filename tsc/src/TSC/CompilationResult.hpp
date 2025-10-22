#pragma once

#include "Diagnostic.hpp"
#include "DiagnosticCode.hpp"
#include "SemanticToken.hpp"
#include "SemanticTokenModifier.hpp"
#include "SemanticTokenType.hpp"
#include "Token.hpp"

namespace ProjectA {
	class CompilationResult {
		public:
			const std::vector<Token>& getTokens() const {
				return tokens;
			}

			const std::vector<SemanticToken>& getSemanticTokens() const {
				return semanticTokens;
			}

			const std::vector<Diagnostic>& getDiagnostics() const {
				return diagnostics;
			}

			void registerToken(TokenType type, Position start, uint32 characterCount) {
				Token token{};
				token.type = type;
				token.start = start;
				token.characterCount = characterCount;

				tokens.emplace_back(token);
			}

			void registerSemanticToken(uint32 tokenIndex, SemanticTokenType type, uint32 modifiers) {
				const Token& token = tokens[tokenIndex];

				registerSemanticToken(token, type, modifiers);
			}

			void registerSemanticToken(const Token& token, SemanticTokenType type, uint32 modifiers) {
				SemanticToken semanticToken{};
				semanticToken.type = type;
				semanticToken.modifiers = modifiers;
				semanticToken.start = token.start;
				semanticToken.characterCount = token.characterCount;

				registerSemanticToken(type, modifiers, token.start, token.characterCount);
			}

			void registerSemanticToken(SemanticTokenType type, uint32 modifiers, Position start, uint32 characterCount) {
				SemanticToken semanticToken{};
				semanticToken.type = type;
				semanticToken.modifiers = modifiers;
				semanticToken.start = start;
				semanticToken.characterCount = characterCount;

				semanticTokens.emplace_back(semanticToken);
			}

			void generateDiagnostic(DiagnosticSeverity severity, const std::string& message, uint32 tokenIndex) {
				generateDiagnostic(severity, message, tokens[tokenIndex]);
			}

			void generateDiagnostic(DiagnosticSeverity severity, const std::string& message, const Token& token) {
				generateDiagnostic(severity, message, token.start, token.getEnd());
			}

			void generateDiagnostic(DiagnosticSeverity severity, const std::string& message, Position start, Position end) {
				Diagnostic diagnostic{};
				diagnostic.severity = severity;
				diagnostic.message = message;
				diagnostic.start = start;
				diagnostic.end = end;

				diagnostics.emplace_back(diagnostic);
			}

		public:
			std::vector<Token> tokens;
			std::vector<SemanticToken> semanticTokens;
			std::vector<Diagnostic> diagnostics;
			std::vector<std::string> identifiers;

			// TODO
			// const char8* identifierBuffer;
	};

	template<DiagnosticCode DiagnosticCode>
	struct generateDiagnostic;

	template<>
	struct generateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER> {
		explicit generateDiagnostic(CompilationResult& result, char8 character, Position start) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::UNEXPECTED_CHARACTER;
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.message = Logger::format("Unexpected character \"[]\".", character);
			diagnostic.start = start;
			diagnostic.end.lineIndex = start.lineIndex;
			diagnostic.end.characterOffset = 1;

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::REDEFINITION> {
		explicit generateDiagnostic(CompilationResult& result, const Token& definitionToken, const Token& redefinitionToken) {
			DiagnosticReference diagnosticReference{};
			diagnosticReference.start = definitionToken.start;
			diagnosticReference.end = definitionToken.getEnd();
			diagnosticReference.message = Logger::format("\"[]\" is defined here.", definitionToken.value);

			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::REDEFINITION;
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.message = Logger::format("Redefinition of \"[]\".", redefinitionToken.value);
			diagnostic.start = redefinitionToken.start;
			diagnostic.end = redefinitionToken.getEnd();
			diagnostic.references.emplace_back(diagnosticReference);

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT> {
		explicit generateDiagnostic(CompilationResult& result, const Token& typeToken, const Token& templateArgumentToken) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.message = Logger::format("\"[]\" cannot be used as a template argument for type \"[]\".", templateArgumentToken.value, typeToken.value);
			diagnostic.start = templateArgumentToken.start;
			diagnostic.end = templateArgumentToken.getEnd();

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE> {
		explicit generateDiagnostic(CompilationResult& result, const Token& typeToken) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::UNDEFINED_TYPE;
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.message = Logger::format("Undefined type \"[]\".", typeToken.value);
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE> {
		explicit generateDiagnostic(CompilationResult& result, const Token& typeToken) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::INCOMPLETE_TYPE;
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.message = Logger::format("Incomplete type \"[]\".", typeToken.value);
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE> {
		explicit generateDiagnostic(CompilationResult& result, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::EMPTY_STRUCTURE;
			diagnostic.severity = DiagnosticSeverity::WARNING;
			diagnostic.message = "Structure is empty.";
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();

			result.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE> {
		explicit generateDiagnostic(CompilationResult& result, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.code = DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE;
			diagnostic.severity = DiagnosticSeverity::WARNING;
			diagnostic.message = "Use camelCase or PascalCase for member identifiers.";
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();

			result.diagnostics.emplace_back(diagnostic);
		}
	};
}