#pragma once

#include "AST.hpp"
#include "Definition.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Language/BaseType.hpp"
#include "PackedType.hpp"
#include "Scope.hpp"
#include "SemanticToken.hpp"
#include "SemanticTokenModifier.hpp"
#include "SemanticTokenType.hpp"
#include "Token.hpp"

namespace ProjectA {
	class CompilationOutput {
		public:
			explicit CompilationOutput() {
				registerDefaultDefinitions();
			}

			uint32 getSize(PackedType type) const {
				DEBUG_ASSERT(!type.hasUserDefinedTemplateArgument());

				if (type.hasUserDefinedType()) {
					// TODO: Return the size from the corresponding definition.
					BREAKPOINT();

					return 0;
				}

				uint32 index = type.typeIndex;

				if (type.templateArgumentIndex > 0) {
					index = type.templateArgumentIndex;
				}

				return TYPE_COMPONENT_COUNTS[type.typeIndex] * TYPE_ALIGNMENTS[index];
			}

			uint32 getAlignment(PackedType type) const {
				DEBUG_ASSERT(!type.hasUserDefinedTemplateArgument());

				if (type.hasUserDefinedType()) {
					// TODO: Return the alignment from the corresponding definition.
					BREAKPOINT();

					return 0;
				}

				if (type.templateArgumentIndex > 0) {
					return TYPE_ALIGNMENTS[type.typeIndex] * TYPE_ALIGNMENTS[type.templateArgumentIndex];
				}

				return TYPE_ALIGNMENTS[type.typeIndex];
			}

			const std::string& getIdentifier(uint32 typeIndex) const {
				DEBUG_ASSERT(typeIndex >= TokenType::USER_DEFINED);
				DEBUG_ASSERT(typeIndex - TokenType::USER_DEFINED < userDefinedIdentifiers.size());

				return userDefinedIdentifiers[typeIndex - TokenType::USER_DEFINED];
			}

			uint64 getLiteral(uint32 literalIndex) const {
				DEBUG_ASSERT(literalIndex < literals.size());

				return literals[literalIndex];
			}

			std::string getFullTypeString(BaseType type, BaseType templateArgument) const {
				std::string typeString = getTypeString(type);

				if (templateArgument == BaseType::OTHER) {
					return typeString;
				}

				return typeString + '<' + getTypeString(templateArgument) + '>';
			}

			std::string getTypeString(BaseType type) const {
				if (type == BaseType::OTHER) {
					return "[user-defined]";
				}

				return RESERVED_KEYWORDS[type + TokenType::TYPE_FLOAT32 - 1];
			}

			void registerToken(TokenType type, DocumentPosition start, uint32 characterCount) {
				Token token{};
				token.type = type;
				token.start = start;
				token.characterCount = characterCount;

				tokens.emplace_back(token);
			}

			void registerSemanticToken(uint32 tokenIndex, SemanticTokenType type, uint32 modifiers) {
				registerSemanticToken(tokens[tokenIndex], type, modifiers);
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
				const Token& token = tokens[tokenIndex];

				Diagnostic diagnostic{};
				diagnostic.start = token.start;
				diagnostic.end = token.getEnd();
				diagnostic.message = message;
				diagnostic.severity = severity;

				diagnostics.emplace_back(diagnostic);
			}

			void openScope(DocumentPosition start) {
				Scope scope{};
				scope.start = start;
				scope.parentScopeIndex = currentScopeIndex;
				scope.definitionIndexOffset = static_cast<uint32>(openScopeDefinitions.size()) - nextScopeDefinitionCount;

				currentScopeIndex = static_cast<uint32>(scopes.size());

				scopes.emplace_back(scope);

				nextScopeDefinitionCount = 0;
			}

			void closeScope(DocumentPosition end) {
				Scope& scope = scopes[currentScopeIndex];
				scope.end = end;
				scope.definitionCount = static_cast<uint32>(openScopeDefinitions.size()) - scope.definitionIndexOffset;

				if (scope.definitionCount > 0) {
					const uint32 insertionOffset = static_cast<uint32>(definitions.size());

					// Allocate space for scope definitions.
					definitions.resize(definitions.size() + scope.definitionCount);

					std::copy_n(&openScopeDefinitions[scope.definitionIndexOffset], scope.definitionCount, &definitions[insertionOffset]);

					openScopeDefinitions.resize(scope.definitionIndexOffset);

					scope.definitionIndexOffset = insertionOffset;
				}

				currentScopeIndex = scope.parentScopeIndex;
			}

			const Definition* getFirstDefinition(const std::string& identifier) const {
				for (uint32 definitionIndex = 0; definitionIndex < openScopeDefinitions.size(); definitionIndex += 1) {
					const Definition& definition = openScopeDefinitions[definitionIndex];

					if (identifier == definition.identifier) {
						return &definition;
					}
				}

				return nullptr;
			}

			void define(PackedType type, SemanticTokenType semanticTokenType, uint32 semanticTokenModifierFlag, uint32 identifierTokenIndex, DocumentPosition end, uint32 size = 0, uint32 alignment = 0) {
				const Token& identifierToken = tokens[identifierTokenIndex];

				Definition definition{};
				definition.end = end;
				definition.type = type;
				definition.semanticTokenType = semanticTokenType;
				definition.semanticTokenModifierFlag = semanticTokenModifierFlag;
				definition.tokenIndex = identifierTokenIndex;
				definition.identifier = getIdentifier(identifierToken.type);

				if (size == 0 && alignment == 0) {
					definition.size = TYPE_COMPONENT_COUNTS[type.typeIndex];
					definition.alignment = TYPE_ALIGNMENTS[type.typeIndex];

					if (type.templateArgumentIndex > 0) {
						definition.size *= TYPE_ALIGNMENTS[type.templateArgumentIndex];
						definition.alignment *= TYPE_ALIGNMENTS[type.templateArgumentIndex];
					}
					else {
						definition.size *= TYPE_ALIGNMENTS[type.typeIndex];
					}
				}
				else {
					definition.size = size;
					definition.alignment = alignment;
				}

				openScopeDefinitions.emplace_back(definition);
			}

			void defineStructure(const ref<StructureStatement>& statement, DocumentPosition end, uint32 size, uint32 alignment) {
				define(PackedType::fromTypeExpression(TokenType::USER_DEFINED, 0), SemanticTokenType::STRUCTURE, 0, statement->identifierTokenIndex, end, size, alignment);
			}

			void define(const ref<ConstantStatement>& statement) {
				const ExpressionReference& typeExpressionReference = ast.getExpressionReference(statement->typeExpressionReferenceIndex);
				const Expression<ExpressionType::TYPE>& typeExpression = ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);
				const Token& identifierToken = tokens[statement->identifierTokenIndex];

				define(typeExpression.type, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT, statement->identifierTokenIndex, identifierToken.getEnd());
			}

			void define(const ref<VariableStatement>& statement) {
				const ExpressionReference& typeExpressionReference = ast.getExpressionReference(statement->typeExpressionReferenceIndex);
				const Expression<ExpressionType::TYPE>& typeExpression = ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);
				const Token& identifierToken = tokens[statement->identifierTokenIndex];
				uint32 modifierFlag = 0;

				if (statement->constant) {
					modifierFlag |= SemanticTokenModifier::CONSTANT;
				}

				define(typeExpression.type, SemanticTokenType::VARIABLE, modifierFlag, statement->identifierTokenIndex, identifierToken.getEnd());
			}

			void defineFunction(const std::string& identifier, DocumentPosition end) {
				Definition definition{};
				definition.end = end;
				definition.type = PackedType(BaseType::OTHER, BaseType::OTHER);
				definition.semanticTokenType = SemanticTokenType::FUNCTION;
				definition.semanticTokenModifierFlag = 0;
				definition.tokenIndex = ~0;
				definition.identifier = identifier;
				definition.size = TYPE_COMPONENT_COUNTS[definition.type.typeIndex];
				definition.alignment = TYPE_ALIGNMENTS[definition.type.typeIndex];

				if (definition.type.templateArgumentIndex > 0) {
					definition.size *= TYPE_ALIGNMENTS[definition.type.templateArgumentIndex];
					definition.alignment *= TYPE_ALIGNMENTS[definition.type.templateArgumentIndex];
				}
				else {
					definition.size *= TYPE_ALIGNMENTS[definition.type.typeIndex];
				}

				openScopeDefinitions.emplace_back(definition);
			}

			std::string getTokenString(const Token& token) const {
				if (token.type == TokenType::LITERAL_FLOATING_POINT || token.type == TokenType::LITERAL_INTEGER) {
					return std::to_string(getLiteral(token.literalIndex));
				}

				return getTokenString(token.type);
			}

			std::string getTokenString(TokenType type) const {
				switch (type) {
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
					case FUNCTION_NORMALIZED: return "normalized";
					case RESERVED_KEYWORD_COUNT: return "[RESERVED]";
					case EQUAL: return "=";
					case ASTERISK_EQUAL: return "*=";
					case SLASH_EQUAL: return "/=";
					case PLUS_EQUAL: return "+=";
					case MINUS_EQUAL: return "-=";
					case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL: return "<<=";
					case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL: return ">>=";
					case AMPERSAND_EQUAL: return "&=";
					case CARET_EQUAL: return "^=";
					case VERTICAL_BAR_EQUAL: return "|=";
					case VERTICAL_BAR: return "|";
					case CARET: return "^";
					case AMPERSAND: return "&";
					case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: return "<<";
					case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: return ">>";
					case EQUAL_EQUAL: return "==";
					case EXCLAMATION_EQUAL: return "!=";
					case LEFT_ANGLE_BRACKET: return "<";
					case LEFT_ANGLE_BRACKET_EQUAL: return "<=";
					case RIGHT_ANGLE_BRACKET: return ">";
					case RIGHT_ANGLE_BRACKET_EQUAL: return ">=";
					case ASTERISK: return "*";
					case SLASH: return "/";
					case PLUS: return "+";
					case MINUS: return "-";
					case EXCLAMATION: return "!";
					case TILDE: return "~";
					case COMMA: return ",";
					case DOT: return ".";
					case LEFT_CURLY_BRACE: return "{";
					case LEFT_PARENTHESIS: return "(";
					case LEFT_SQUARE_BRACKET: return "[";
					case RIGHT_CURLY_BRACE: return "}";
					case RIGHT_PARENTHESIS: return ")";
					case RIGHT_SQUARE_BRACKET: return "]";
					case END_OF_FILE: return "EOF";
					case END_OF_LINE: return "EOL";
					case USER_DEFINED:
					default: return getIdentifier(type);
				}
			}

		private:
			void registerDefaultDefinitions() {
				for (uint32 functionIndex = FIRST_FUNCTION_INDEX; functionIndex <= LAST_FUNCTION_INDEX; functionIndex += 1) {
					const std::string functionIdentifier = RESERVED_KEYWORDS[functionIndex];

					defineFunction(functionIdentifier, {0, 0});

					nextScopeDefinitionCount += 1;
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

			std::vector<uint64> literals;
			std::vector<std::string> userDefinedIdentifiers;

			std::vector<Definition> openScopeDefinitions;
			uint32 nextScopeDefinitionCount = 0;
			std::vector<Definition> definitions;
			std::vector<Scope> scopes;
			uint32 currentScopeIndex = UNDEFINED_SCOPE;
	};
}