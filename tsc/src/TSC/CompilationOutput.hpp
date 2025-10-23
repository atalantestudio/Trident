#pragma once

#include "AST.hpp"
#include "CompletionItem.hpp"
#include "DefinitionStack.hpp"
#include "Diagnostic.hpp"
#include "DiagnosticCode.hpp"
#include "SemanticToken.hpp"
#include "SemanticTokenModifier.hpp"
#include "SemanticTokenType.hpp"
#include "Token.hpp"

namespace ProjectA {
	class CompilationOutput {
		public:
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

			void registerCompletionItem(uint32 tokenIndex, CompletionItemKind kind) {
				const Token& token = tokens[tokenIndex];

				CompletionItem completionItem{};
				completionItem.label = token.value;
				completionItem.labelSize = token.characterCount;
				completionItem.kind = kind;
				completionItem.end = token.getEnd();

				completionItems.emplace_back(completionItem);
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
			uint32 currentDefinitionStackIndex;

			std::vector<CompletionItem> completionItems;
	};

	template<DiagnosticCode DiagnosticCode>
	struct generateDiagnostic;

	template<>
	struct generateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER> {
		explicit generateDiagnostic(CompilationOutput& output, char8 character, DocumentPosition start) {
			Diagnostic diagnostic{};
			diagnostic.start = start;
			diagnostic.end.lineIndex = start.lineIndex;
			diagnostic.end.characterOffset = 1;
			diagnostic.message = Logger::format("Unexpected character \"[]\".", character);
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::UNEXPECTED_CHARACTER;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::REDEFINITION> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& definitionToken, const Token& redefinitionToken) {
			DiagnosticReference diagnosticReference{};
			diagnosticReference.start = definitionToken.start;
			diagnosticReference.end = definitionToken.getEnd();
			diagnosticReference.message = Logger::format("\"[]\" is defined here.", getTokenString(definitionToken));

			Diagnostic diagnostic{};
			diagnostic.start = redefinitionToken.start;
			diagnostic.end = redefinitionToken.getEnd();
			diagnostic.message = Logger::format("Redefinition of \"[]\".", getTokenString(redefinitionToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::REDEFINITION;
			diagnostic.references.emplace_back(diagnosticReference);

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& typeToken, const Token& templateArgumentToken) {
			Diagnostic diagnostic{};
			diagnostic.start = templateArgumentToken.start;
			diagnostic.end = templateArgumentToken.getEnd();
			diagnostic.message = Logger::format("\"[]\" cannot be used as a template argument for type \"[]\".", getTokenString(templateArgumentToken), getTokenString(typeToken));
			diagnostic.severity = DiagnosticSeverity::ERROR;
			diagnostic.code = DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;

			output.diagnostics.emplace_back(diagnostic);
		}
	};

	template<>
	struct generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& typeToken) {
			Diagnostic diagnostic{};
			diagnostic.start = typeToken.start;
			diagnostic.end = typeToken.getEnd();
			diagnostic.message = Logger::format("Undefined type \"[]\".", getTokenString(typeToken));
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
			diagnostic.message = Logger::format("Incomplete type \"[]\".", getTokenString(typeToken));
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
			diagnostic.message = Logger::format("Duplicate annotation \"[]\".", getTokenString(identifierToken));
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
			diagnostic.message = Logger::format("Missing value for annotation \"[]\".", getTokenString(identifierToken));
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
			diagnostic.message = Logger::format("Undefined identifier \"[]\".", getTokenString(identifierToken));
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
	struct generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Structure is empty.";
			diagnostic.severity = DiagnosticSeverity::WARNING;
			diagnostic.code = DiagnosticCode::EMPTY_STRUCTURE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};
}