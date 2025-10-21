#pragma once

#include "Diagnostic.hpp"
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
				const Token& token = tokens[tokenIndex];

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
}