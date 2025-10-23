#pragma once

#include "AST.hpp"
#include "CompletionItem.hpp"
#include "DefinitionStack.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "SemanticToken.hpp"
#include "SemanticTokenModifier.hpp"
#include "SemanticTokenType.hpp"
#include "Token.hpp"

namespace ProjectA {
	class CompilationOutput {
		public:
			explicit CompilationOutput() {
				registerDefaultCompletionItems();
			}

			void registerToken(TokenType type, DocumentPosition start, uint32 characterCount) {
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

			void registerSemanticToken(SemanticTokenType type, uint32 modifiers, DocumentPosition start, uint32 characterCount) {
				SemanticToken semanticToken{};
				semanticToken.type = type;
				semanticToken.modifiers = modifiers;
				semanticToken.start = start;
				semanticToken.characterCount = characterCount;

				semanticTokens.emplace_back(semanticToken);
			}

			void generateDiagnostic(uint32 tokenIndex, const std::string& message, DiagnosticSeverity severity) {
				generateDiagnostic(tokens[tokenIndex], message, severity);
			}

			void generateDiagnostic(const Token& token, const std::string& message, DiagnosticSeverity severity) {
				generateDiagnostic(token.start, token.getEnd(), message, severity);
			}

			void generateDiagnostic(DocumentPosition start, DocumentPosition end, const std::string& message, DiagnosticSeverity severity) {
				Diagnostic diagnostic{};
				diagnostic.start = start;
				diagnostic.end = end;
				diagnostic.message = message;
				diagnostic.severity = severity;

				diagnostics.emplace_back(diagnostic);
			}

			const DefinitionStack& getCurrentDefinitionStack() const {
				if (currentDefinitionStackIndex >= definitionStacks.size()) {
					BREAKPOINT();
				}

				return definitionStacks[currentDefinitionStackIndex];
			}

			void openDefinitionStack() {
				DefinitionStack definitionStack{};

				definitionStacks.emplace_back(definitionStack);

				currentDefinitionStackIndex = static_cast<uint32>(definitionStacks.size()) - 1;
			}

			void closeDefinitionStack() {
				if (currentDefinitionStackIndex == 0) {
					BREAKPOINT();
				}

				currentDefinitionStackIndex -= 1;
			}

			void define(uint32 tokenIndex, uint32 structureStatementIndex = ~0) {
				const Token& token = tokens[tokenIndex];

				Definition definition{};
				definition.identifier = token.value;
				definition.identifierSize = token.characterCount;
				definition.structureStatementIndex = structureStatementIndex;

				definitionStacks[currentDefinitionStackIndex].stack.emplace_back(definition);
			}

			bool isDefined(const Token& token) const {
				for (sint32 definitionStackIndex = static_cast<sint32>(currentDefinitionStackIndex); definitionStackIndex >= 0; definitionStackIndex -= 1) {
					const DefinitionStack& definitionStack = definitionStacks[definitionStackIndex];

					for (const Definition& definition : definitionStack.stack) {
						uint32 size = token.characterCount;

						if (definition.identifierSize > size) {
							size = definition.identifierSize;
						}

						if (std::strncmp(token.value, definition.identifier, size) == 0) {
							return true;
						}
					}
				}

				return false;
			}

			void registerCompletionItem(const std::string& label, CompletionItemKind kind) {
				CompletionItem completionItem{};
				completionItem.label = label;
				completionItem.kind = kind;
				completionItem.end = {0, 0};

				completionItems.emplace_back(completionItem);
			}

		private:
			void registerDefaultCompletionItems() {
				for (uint32 functionIndex = FIRST_FUNCTION_INDEX; functionIndex <= LAST_FUNCTION_INDEX; functionIndex += 1) {
					const std::string functionIdentifier = RESERVED_KEYWORDS[functionIndex];

					registerCompletionItem(functionIdentifier, CompletionItemKind::FUNCTION);
				}
			}

		public:
			// Source code.
			// Must be conserved to show advanced errors.
			std::string text;

			AST ast;

			std::vector<Token> tokens;
			std::vector<SemanticToken> semanticTokens;
			std::vector<Diagnostic> diagnostics;

			// TODO
			std::vector<std::string> identifiers;

			std::vector<DefinitionStack> definitionStacks;
			uint32 currentDefinitionStackIndex = 0;

			std::vector<CompletionItem> completionItems;
	};
}