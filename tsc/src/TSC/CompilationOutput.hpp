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

			const std::string& getIdentifier(uint32 typeIndex) const {
				DEBUG_ASSERT(typeIndex >= TokenType::USER_DEFINED);

				return userDefinedIdentifiers[typeIndex - TokenType::USER_DEFINED];
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
				DEBUG_ASSERT(currentDefinitionStackIndex < definitionStacks.size());

				return definitionStacks[currentDefinitionStackIndex];
			}

			void openDefinitionStack() {
				DefinitionStack definitionStack{};

				definitionStacks.emplace_back(definitionStack);

				currentDefinitionStackIndex = static_cast<uint32>(definitionStacks.size()) - 1;
			}

			void closeDefinitionStack() {
				DEBUG_ASSERT(currentDefinitionStackIndex > 0);

				currentDefinitionStackIndex -= 1;
			}

			void define(uint32 tokenIndex, uint32 structureStatementIndex = ~0) {
				const Token& token = tokens[tokenIndex];

				Definition definition{};
				definition.identifier = getIdentifier(token.type);
				definition.structureStatementIndex = structureStatementIndex;

				definitionStacks[currentDefinitionStackIndex].stack.emplace_back(definition);
			}

			bool isDefined(const Token& token) const {
				const std::string& identifier = getIdentifier(token.type);

				for (sint32 definitionStackIndex = static_cast<sint32>(currentDefinitionStackIndex); definitionStackIndex >= 0; definitionStackIndex -= 1) {
					const DefinitionStack& definitionStack = definitionStacks[definitionStackIndex];

					for (const Definition& definition : definitionStack.stack) {
						if (identifier == definition.identifier) {
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

			std::string getTokenString(const Token& token) const {
				switch (token.type) {
					case KEYWORD_AND: return "AND";
					case KEYWORD_BREAK: return "BREAK";
					case KEYWORD_IF: return "IF";
					case KEYWORD_NEXT: return "NEXT";
					case KEYWORD_OR: return "OR";
					case KEYWORD_RETURN: return "RETURN";
					case KEYWORD_WHILE: return "WHILE";
					case KEYWORD_XOR: return "XOR";
					case TYPE_FLOAT32: return "float32";
					case TYPE_FLOAT64: return "float64";
					case TYPE_SINT32: return "sint32";
					case TYPE_UINT32: return "uint32";
					case TYPE_VEC2: return "vec2";
					case TYPE_VEC3: return "vec3";
					case TYPE_VEC4: return "vec4";
					case TYPE_MAT2X2: return "mat2x2";
					case TYPE_MAT2X3: return "mat2x3";
					case TYPE_MAT2X4: return "mat2x4";
					case TYPE_MAT3X2: return "mat3x2";
					case TYPE_MAT3X3: return "mat3x3";
					case TYPE_MAT3X4: return "mat3x4";
					case TYPE_MAT4X2: return "mat4x2";
					case TYPE_MAT4X3: return "mat4x3";
					case TYPE_MAT4X4: return "mat4x4";
					case FUNCTION_NORMALIZED: return "\"normalized\"";
					case RESERVED_KEYWORD_COUNT: return "[reserved keyword]";
					case EQUAL: return "'='";
					case ASTERISK_EQUAL: return "'*='";
					case SLASH_EQUAL: return "'/='";
					case PLUS_EQUAL: return "'+='";
					case MINUS_EQUAL: return "'-='";
					case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL: return "'<<='";
					case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL: return "'>>='";
					case AMPERSAND_EQUAL: return "'&='";
					case CARET_EQUAL: return "'^='";
					case VERTICAL_BAR_EQUAL: return "'|='";
					case VERTICAL_BAR: return "'|'";
					case CARET: return "'^'";
					case AMPERSAND: return "'&'";
					case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: return "'<<'";
					case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: return "'>>'";
					case EQUAL_EQUAL: return "'=='";
					case EXCLAMATION_EQUAL: return "'!='";
					case LEFT_ANGLE_BRACKET: return "'<'";
					case LEFT_ANGLE_BRACKET_EQUAL: return "'<='";
					case RIGHT_ANGLE_BRACKET: return "'>'";
					case RIGHT_ANGLE_BRACKET_EQUAL: return "'>='";
					case ASTERISK: return "'*'";
					case SLASH: return "'/'";
					case PLUS: return "'+'";
					case MINUS: return "'-'";
					case EXCLAMATION: return "'!'";
					case TILDE: return "'~'";
					case COMMA: return "','";
					case DOT: return "'.'";
					case LEFT_CURLY_BRACE: return "'{'";
					case LEFT_PARENTHESIS: return "'('";
					case LEFT_SQUARE_BRACKET: return "'['";
					case RIGHT_CURLY_BRACE: return "'}'";
					case RIGHT_PARENTHESIS: return "')'";
					case RIGHT_SQUARE_BRACKET: return "']'";
					case END_OF_FILE: return "EOF";
					case END_OF_LINE: return "EOL";
					case LITERAL_FLOATING_POINT:
					case LITERAL_INTEGER_BASE_2:
					case LITERAL_INTEGER_BASE_8:
					case LITERAL_INTEGER_BASE_10:
					case LITERAL_INTEGER_BASE_16: return std::string(token.value, token.characterCount);
					case USER_DEFINED:
					default: return getIdentifier(token.type);
				}
			}

		private:
			void registerDefaultCompletionItems() {
				for (uint32 functionIndex = FIRST_FUNCTION_INDEX; functionIndex <= LAST_FUNCTION_INDEX; functionIndex += 1) {
					const std::string functionIdentifier = RESERVED_KEYWORDS[functionIndex];

					registerCompletionItem(functionIdentifier, CompletionItemKind::FUNCTION);
				}
			}

		public:
			// Source code. Must be conserved to show advanced errors.
			// TODO: Is this the preprocessed source code?
			std::string text;

			AST ast;
			std::vector<Token> tokens;
			std::vector<SemanticToken> semanticTokens;
			std::vector<Diagnostic> diagnostics;
			std::vector<std::string> userDefinedIdentifiers;
			std::vector<DefinitionStack> definitionStacks = {{}};
			uint32 currentDefinitionStackIndex = 0;
			std::vector<CompletionItem> completionItems;
	};
}