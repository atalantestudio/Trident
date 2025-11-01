#pragma once

#include "AST.hpp"
#include "Definition.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Function.hpp"
#include "Language/Type.hpp"
#include "PackedType.hpp"
#include "Reference.hpp"
#include "Scope.hpp"
#include "SemanticToken.hpp"
#include "SemanticTokenType.hpp"
#include "Token.hpp"
#include "UserDefinedType.hpp"

namespace ProjectA {
	class CompilationOutput {
		public:
			explicit CompilationOutput() {
				// registerDefaultDefinitions();
			}

			const std::string& _emscripten_getIdentifier(uint32 identifierIndex) const {
				DEBUG_ASSERT(identifierIndex < identifiers.size());

				return identifiers[identifierIndex];
			}

			uint32 _emscripten_getSize(uint32 typeIndex, uint32 templateArgumentIndex) const {
				return getSize(PackedType(static_cast<Type>(typeIndex), static_cast<Type>(templateArgumentIndex)));
			}

			uint32 _emscripten_getAlignment(uint32 typeIndex, uint32 templateArgumentIndex) const {
				return getAlignment(PackedType(static_cast<Type>(typeIndex), static_cast<Type>(templateArgumentIndex)));
			}

			std::string _emscripten_getPackedTypeString(uint32 typeIndex, uint32 templateArgumentIndex) const {
				return getPackedTypeString(PackedType(static_cast<Type>(typeIndex), static_cast<Type>(templateArgumentIndex)));
			}

			uint32 getSize(PackedType type) const {
				DEBUG_ASSERT(!type.hasUserDefinedTemplateArgument());

				if (type.hasUserDefinedType()) {
					const UserDefinedType* userDefinedType = getUserDefinedType(type.typeIndex);

					if (userDefinedType == nullptr) {
						return 0;
					}

					return userDefinedType->size;
				}

				uint32 index = type.typeIndex;

				if (type.templateArgumentIndex > Type::NONE) {
					index = type.templateArgumentIndex;
				}

				return TYPE_ALIGNED_COMPONENT_COUNTS[type.typeIndex] * TYPE_ALIGNMENTS[index];
			}

			uint32 getAlignment(PackedType type) const {
				DEBUG_ASSERT(!type.hasUserDefinedTemplateArgument());

				if (type.hasUserDefinedType()) {
					const UserDefinedType* userDefinedType = getUserDefinedType(type.typeIndex);

					if (userDefinedType == nullptr) {
						return 0;
					}

					return userDefinedType->alignment;
				}

				if (type.templateArgumentIndex > Type::NONE) {
					return TYPE_ALIGNMENTS[type.typeIndex] * TYPE_ALIGNMENTS[type.templateArgumentIndex];
				}

				return TYPE_ALIGNMENTS[type.typeIndex];
			}

			const UserDefinedType* getUserDefinedType(Type typeIndex) const {
				const uint32 index = typeIndex - Type::USER_DEFINED;

				if (index >= userDefinedTypes.size()) {
					return nullptr;
				}

				return &userDefinedTypes[index];
			}

			bool isValidUserDefinedType(PackedType type) const {
				DEBUG_ASSERT(type.hasUserDefinedType());

				return getUserDefinedType(type.typeIndex) != nullptr;
			}

			uint32 getComponentCount(PackedType type) const {
				if (type.hasUserDefinedType()) {
					return getUserDefinedType(type.typeIndex)->constructorArgumentCount;
				}

				uint32 count = TYPE_COMPONENT_COUNTS[type.typeIndex];

				if (type.templateArgumentIndex > Type::NONE) {
					count *= TYPE_COMPONENT_COUNTS[type.templateArgumentIndex];
				}

				return count;
			}

			// TODO: Also check return type.
			uint32 getFunctionIndex(uint32 identifierIndex) const {
				for (uint32 functionIndex = 0; functionIndex < functions.size(); functionIndex += 1) {
					if (functions[functionIndex].identifierIndex == identifierIndex) {
						return functionIndex;
					}
				}

				return ~0;
			}

			uint32 getUserDefinedTypeIndex(uint32 identifierIndex) const {
				for (uint32 userDefinedTypeIndex = 0; userDefinedTypeIndex < userDefinedTypes.size(); userDefinedTypeIndex += 1) {
					if (userDefinedTypes[userDefinedTypeIndex].identifierIndex == identifierIndex) {
						return userDefinedTypeIndex;
					}
				}

				return ~0;
			}

			std::string getIdentifier(const Token& token) const {
				if (token.type >= FIRST_FUNCTION_INDEX && token.type <= LAST_FUNCTION_INDEX) {
					return RESERVED_KEYWORDS[token.type - FIRST_FUNCTION_INDEX];
				}

				DEBUG_ASSERT(token.type == TokenType::IDENTIFIER);
				DEBUG_ASSERT(token.index < identifiers.size());

				return identifiers[token.index];
			}

			uint64 getLiteral(uint32 index) const {
				DEBUG_ASSERT(index < literals.size());

				return literals[index];
			}

			std::string getPackedTypeString(PackedType packedType) const {
				std::string typeString = getTypeString(packedType.typeIndex);

				if (packedType.templateArgumentIndex > Type::NONE) {
					typeString += '<' + getTypeString(packedType.templateArgumentIndex) + '>';
				}

				return typeString;
			}

			std::string getTypeString(Type packedType) const {
				if (packedType == Type::NONE) {
					return "";
				}

				if (packedType < Type::USER_DEFINED) {
					return RESERVED_KEYWORDS[packedType + TokenType::TYPE_FLOAT32 - 1];
				}

				const UserDefinedType* userDefinedType = getUserDefinedType(packedType);

				DEBUG_ASSERT(userDefinedType != nullptr);
				DEBUG_ASSERT(userDefinedType->identifierIndex < identifiers.size());

				return identifiers[userDefinedType->identifierIndex];
			}

			void registerToken(TokenType type, DocumentPosition start, uint32 characterCount) {
				Token token{};
				token.type = type;
				token.start = start;
				token.characterCount = characterCount;

				tokens.emplace_back(token);
			}

			void registerSemanticToken(SemanticTokenType type, uint32 tokenIndex) {
				const Token& token = tokens[tokenIndex];

				registerSemanticToken(type, token.start, token.getEnd());
			}

			void registerSemanticToken(SemanticTokenType type, DocumentPosition start, DocumentPosition end) {
				SemanticToken semanticToken{};
				semanticToken.type = type;
				semanticToken.range.start = start;
				semanticToken.range.end = end;

				semanticTokens.emplace_back(semanticToken);
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

			// Compares the identifier string to the identifiers of the open definitions; that is, the definitions whose parent scopes are not yet closed.
			// Returns ~0 if no corresponding definition is found.
			uint32 getDefinitionIndex(uint32 identifierTokenIndex) const {
				const Token& identifierToken = tokens[identifierTokenIndex];

				for (uint32 definitionIndex = 0; definitionIndex < openScopeDefinitions.size(); definitionIndex += 1) {
					const Definition& definition = openScopeDefinitions[definitionIndex];

					if (identifierToken.index == definition.identifierIndex) {
						return definitionIndex;
					}
				}

				return ~0;
			}

			Definition& define(PackedType type, SemanticTokenType semanticTokenType, uint32 identifierTokenIndex, DocumentPosition end, uint32 size = 0, uint32 alignment = 0) {
				const Token& identifierToken = tokens[identifierTokenIndex];

				Definition definition{};
				definition.identifierRange.start = identifierToken.start;
				definition.identifierRange.end = identifierToken.getEnd();
				definition.end = end;
				definition.type = type;
				definition.semanticTokenType = semanticTokenType;
				definition.tokenIndex = identifierTokenIndex;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				return openScopeDefinitions.emplace_back(definition);
			}

			Definition& defineStructure(const Statement<StatementType::STRUCTURE>& statement) {
				const Token& identifierToken = tokens[statement.identifierTokenIndex];
				const PackedType type = PackedType::fromTypeExpression(TokenType::IDENTIFIER, static_cast<uint32>(userDefinedTypes.size()), 0);

				registerSemanticToken(SemanticTokenType::STRUCTURE, statement.identifierTokenIndex);

				UserDefinedType userDefinedType{};
				userDefinedType.identifierIndex = identifierToken.index;
				userDefinedType.constructorArgumentCount = statement.memberCount;

				userDefinedTypes.emplace_back(userDefinedType);

				Definition definition{};
				definition.identifierRange.start = identifierToken.start;
				definition.identifierRange.end = identifierToken.getEnd();
				definition.end = statement.end;
				definition.type = type;
				definition.semanticTokenType = SemanticTokenType::STRUCTURE;
				definition.tokenIndex = statement.identifierTokenIndex;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				return openScopeDefinitions.emplace_back(definition);
			}

			Definition& defineMember(const Statement<StatementType::MEMBER>& statement, const Expression<ExpressionType::TYPE>& typeExpression) {
				const Token& identifierToken = tokens[statement.identifierTokenIndex];

				Definition definition{};
				definition.identifierRange.start = identifierToken.start;
				definition.identifierRange.end = identifierToken.getEnd();
				definition.end = definition.identifierRange.end;
				definition.type = typeExpression.type;
				definition.semanticTokenType = SemanticTokenType::MEMBER;
				definition.tokenIndex = statement.identifierTokenIndex;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				return openScopeDefinitions.emplace_back(definition);
			}

			Definition& defineConstant(const Statement<StatementType::CONSTANT>& statement, const ExpressionReference& typeExpressionReference) {
				DEBUG_ASSERT(typeExpressionReference.type == ExpressionType::TYPE);

				const Expression<ExpressionType::TYPE>& typeExpression = ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);
				const Token& identifierToken = tokens[statement.identifierTokenIndex];

				Definition definition{};
				definition.identifierRange.start = identifierToken.start;
				definition.identifierRange.end = identifierToken.getEnd();
				definition.end = identifierToken.getEnd();
				definition.type = typeExpression.type;
				definition.semanticTokenType = SemanticTokenType::CONSTANT;
				definition.tokenIndex = statement.identifierTokenIndex;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				Definition& result = openScopeDefinitions.emplace_back(definition);

				createReference(static_cast<uint32>(openScopeDefinitions.size() - 1), statement.identifierTokenIndex);

				return result;
			}

			Definition& defineVariableOrConstant(const Statement<StatementType::VARIABLE>& statement, const Expression<ExpressionType::TYPE>& typeExpression) {
				const Token& identifierToken = tokens[statement.identifierTokenIndex];
				SemanticTokenType semanticTokenType = SemanticTokenType::VARIABLE;

				if (statement.constant) {
					semanticTokenType = SemanticTokenType::CONSTANT;
				}

				return define(typeExpression.type, semanticTokenType, statement.identifierTokenIndex, identifierToken.getEnd());
			}

			Definition& defineFunction(const Statement<StatementType::FUNCTION>& statement, uint32 parameterTypeExpressionOffset, DocumentPosition end) {
				const Token& identifierToken = tokens[statement.identifierTokenIndex];

				registerFunctionOverload(statement, parameterTypeExpressionOffset);

				Definition definition{};
				definition.identifierRange.start = end;
				definition.identifierRange.end = end;
				definition.end = end;
				definition.type = PackedType(Type::FUNCTION, Type::NONE);
				definition.semanticTokenType = SemanticTokenType::FUNCTION;
				definition.tokenIndex = ~0;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				return openScopeDefinitions.emplace_back(definition);
			}

			Definition& defineFunction(const Token& identifierToken, DocumentPosition end) {
				Definition definition{};
				definition.identifierRange.start = identifierToken.start;
				definition.identifierRange.end = identifierToken.getEnd();
				definition.end = end;
				definition.type = PackedType(Type::FUNCTION, Type::NONE);
				definition.semanticTokenType = SemanticTokenType::FUNCTION;
				definition.tokenIndex = ~0;
				definition.identifierIndex = identifierToken.index;
				definition.evaluatedInitializer = 0;

				return openScopeDefinitions.emplace_back(definition);
			}

			void createReference(uint32 definitionIndex, uint32 identifierTokenIndex) {
				const Definition& definition = getOpenScopeDefinition(definitionIndex);
				const Token& identifierToken = tokens[identifierTokenIndex];

				registerSemanticToken(definition.semanticTokenType, identifierTokenIndex);

				Reference reference{};
				reference.definitionIndex = definitionIndex;
				reference.identifierIndex = identifierToken.index;
				reference.identifierRange.start = identifierToken.start;
				reference.identifierRange.end = identifierToken.getEnd();

				references.emplace_back(reference);
			}

			std::string getTokenString(const Token& token) const {
				if (token.type == TokenType::LITERAL_FLOATING_POINT || token.type == TokenType::LITERAL_INTEGER) {
					return std::to_string(getLiteral(token.index));
				}

				if (token.type == TokenType::IDENTIFIER) {
					DEBUG_ASSERT(token.index < identifiers.size());

					return identifiers[token.index];
				}

				return getNonIdentifierTokenString(token.type);
			}

			std::string getNonIdentifierTokenString(TokenType type) const {
				switch (type) {
					case TokenType::KEYWORD_AND: return "AND";
					case TokenType::KEYWORD_BREAK: return "BREAK";
					case TokenType::KEYWORD_IF: return "IF";
					case TokenType::KEYWORD_NEXT: return "NEXT";
					case TokenType::KEYWORD_OR: return "OR";
					case TokenType::KEYWORD_RETURN: return "RETURN";
					case TokenType::KEYWORD_WHILE: return "WHILE";
					case TokenType::KEYWORD_XOR: return "XOR";
					case TokenType::TYPE_FLOAT32: return "float32";
					case TokenType::TYPE_FLOAT64: return "float64";
					case TokenType::TYPE_SINT32: return "sint32";
					case TokenType::TYPE_UINT32: return "uint32";
					case TokenType::TYPE_VEC2: return "vec2";
					case TokenType::TYPE_VEC3: return "vec3";
					case TokenType::TYPE_VEC4: return "vec4";
					case TokenType::TYPE_MAT2X2: return "mat2x2";
					case TokenType::TYPE_MAT2X3: return "mat2x3";
					case TokenType::TYPE_MAT2X4: return "mat2x4";
					case TokenType::TYPE_MAT3X2: return "mat3x2";
					case TokenType::TYPE_MAT3X3: return "mat3x3";
					case TokenType::TYPE_MAT3X4: return "mat3x4";
					case TokenType::TYPE_MAT4X2: return "mat4x2";
					case TokenType::TYPE_MAT4X3: return "mat4x3";
					case TokenType::TYPE_MAT4X4: return "mat4x4";
					case TokenType::FUNCTION_NORMALIZED: return "normalized";
					case TokenType::RESERVED_KEYWORD_COUNT: return "[RESERVED]";
					case TokenType::EQUAL: return "=";
					case TokenType::ASTERISK_EQUAL: return "*=";
					case TokenType::SLASH_EQUAL: return "/=";
					case TokenType::PLUS_EQUAL: return "+=";
					case TokenType::MINUS_EQUAL: return "-=";
					case TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL: return "<<=";
					case TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL: return ">>=";
					case TokenType::AMPERSAND_EQUAL: return "&=";
					case TokenType::CARET_EQUAL: return "^=";
					case TokenType::VERTICAL_BAR_EQUAL: return "|=";
					case TokenType::VERTICAL_BAR: return "|";
					case TokenType::CARET: return "^";
					case TokenType::AMPERSAND: return "&";
					case TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: return "<<";
					case TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: return ">>";
					case TokenType::EQUAL_EQUAL: return "==";
					case TokenType::EXCLAMATION_EQUAL: return "!=";
					case TokenType::LEFT_ANGLE_BRACKET: return "<";
					case TokenType::LEFT_ANGLE_BRACKET_EQUAL: return "<=";
					case TokenType::RIGHT_ANGLE_BRACKET: return ">";
					case TokenType::RIGHT_ANGLE_BRACKET_EQUAL: return ">=";
					case TokenType::ASTERISK: return "*";
					case TokenType::SLASH: return "/";
					case TokenType::PLUS: return "+";
					case TokenType::MINUS: return "-";
					case TokenType::EXCLAMATION: return "!";
					case TokenType::TILDE: return "~";
					case TokenType::COMMA: return ",";
					case TokenType::DOT: return ".";
					case TokenType::LEFT_CURLY_BRACE: return "{";
					case TokenType::LEFT_PARENTHESIS: return "(";
					case TokenType::LEFT_SQUARE_BRACKET: return "[";
					case TokenType::RIGHT_CURLY_BRACE: return "}";
					case TokenType::RIGHT_PARENTHESIS: return ")";
					case TokenType::RIGHT_SQUARE_BRACKET: return "]";
					case TokenType::END_OF_LINE: return "EOL";
					default: {
						DEBUG_ASSERT(false);

						return "";
					}
				}
			}

			uint32 getOpenScopeDefinitionCount() const {
				return static_cast<uint32>(openScopeDefinitions.size());
			}

			const Definition& getOpenScopeDefinition(uint32 definitionIndex) const {
				DEBUG_ASSERT(definitionIndex < openScopeDefinitions.size());

				return openScopeDefinitions[definitionIndex];
			}

			UserDefinedType& getLastUserDefinedType() {
				return userDefinedTypes.back();
			}

		private:
			/* void registerDefaultDefinitions() {
				for (uint32 functionIndex = FIRST_FUNCTION_INDEX; functionIndex <= LAST_FUNCTION_INDEX; functionIndex += 1) {
					const std::string functionIdentifier = RESERVED_KEYWORDS[functionIndex];

					defineFunction(functionIdentifier, {0, 0});

					nextScopeDefinitionCount += 1;
				}
			} */

			void registerFunctionOverload(const Statement<StatementType::FUNCTION>& statement, uint32 parameterTypeExpressionOffset) {
				const Token& identifierToken = tokens[statement.identifierTokenIndex];
				const uint32 identifierIndex = identifierToken.index;

				uint32 functionIndex = getFunctionIndex(identifierIndex);

				if (functionIndex == ~0) {
					functionIndex = registerFunction(identifierIndex);
				}

				// TODO: Compare with other overloads.
				FunctionOverload functionOverload{};
				functionOverload.parameters.resize(statement.parameterCount);

				uint32 parameterIndex = 0;

				while (parameterIndex < statement.parameterCount) {
					const ExpressionReference& parameterTypeExpressionReference = ast.getExpressionReference(parameterTypeExpressionOffset + parameterIndex);
					const Expression<ExpressionType::TYPE>& parameterTypeExpression = ast.getExpression<ExpressionType::TYPE>(parameterTypeExpressionReference.byteOffset);

					functionOverload.parameters[parameterIndex].type = parameterTypeExpression.type;

					parameterIndex += 1;
				}

				functions[functionIndex].overloads.emplace_back(functionOverload);
			}

			// Registers a new function and returns its index.
			uint32 registerFunction(uint32 identifierIndex) {
				const uint32 functionIndex = static_cast<uint32>(functions.size());

				Function function{};
				function.identifierIndex = identifierIndex;

				functions.emplace_back(function);

				return functionIndex;
			}

		public:
			// Source code. Must be conserved to show advanced errors.
			// TODO: Is this the preprocessed source code?
			std::string text;

			// Language and user-defined functions.
			std::vector<Function> functions;

			AST ast;
			std::vector<Token> tokens;
			std::vector<SemanticToken> semanticTokens;
			std::vector<Diagnostic> diagnostics;

			std::vector<uint64> literals;

			std::vector<std::string> identifiers;
			std::vector<UserDefinedType> userDefinedTypes;

			std::vector<Definition> openScopeDefinitions;
			uint32 nextScopeDefinitionCount = 0;
			std::vector<Definition> definitions;

			// TODO: Finish reference system.
			std::vector<Reference> references;

			std::vector<Scope> scopes;
			uint32 currentScopeIndex = UNDEFINED_SCOPE;
	};
}