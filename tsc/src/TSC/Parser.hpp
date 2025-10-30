#pragma once

#include "CompilationOutput.hpp"
#include "Diagnostic/DiagnosticCode.hpp"
#include "DocumentPosition.hpp"
#include "SemanticAnalyzer.hpp"
#include "Token.hpp"

namespace ProjectA {
	class Parser {
		public:
			void parse(CompilationOutput* output);

		private:
			DiagnosticCode parseGlobalScopeStatement();
			DiagnosticCode parseStructureStatement();
			DiagnosticCode parseMemberStatement(uint32 structureStatementIndex, ref<StructureStatement>& structureStatement);
			DiagnosticCode parseConstantOrFunctionStatement();
			DiagnosticCode parseFunctionStatement();
			DiagnosticCode parseParameterStatement();

			DiagnosticCode parseLocalScopeStatement();
			DiagnosticCode parseScopeStatement();
			DiagnosticCode parseIfStatementStart();
			DiagnosticCode parseReturnStatement();
			DiagnosticCode parseBreakStatement();
			DiagnosticCode parseNextStatement();
			DiagnosticCode parseWhileStatementStart();
			DiagnosticCode parseExpressionStatement();
			DiagnosticCode parseIfOrWhileStatementEnd(uint32& scopeStatementIndex);
			DiagnosticCode parseVariableStatement();

			DiagnosticCode parseTypeExpression();

			DiagnosticCode parseAssignmentOrCompoundExpression();
			DiagnosticCode parseLogicalOrExpression();
			DiagnosticCode parseLogicalXorExpression();
			DiagnosticCode parseLogicalAndExpression();
			DiagnosticCode parseBitwiseOrExpression();
			DiagnosticCode parseBitwiseXorExpression();
			DiagnosticCode parseBitwiseAndExpression();
			DiagnosticCode parseEqualityExpression();
			DiagnosticCode parseRelationalExpression();
			DiagnosticCode parseBitwiseShiftExpression();
			DiagnosticCode parseAdditiveExpression();
			DiagnosticCode parseMultiplicativeExpression();
			DiagnosticCode parseUnaryExpression();
			DiagnosticCode parsePrimaryExpression();

			DiagnosticCode parseIdentifierExpression();
			DiagnosticCode parseMemberAccessExpression();
			DiagnosticCode parseCallOrGroupExpression(uint32 insertionIndex);
			DiagnosticCode parseLiteralExpression();
			DiagnosticCode parseGroupExpression();

			DiagnosticCode parseAnnotationBlocks();

			uint32 getLastStatementIndex() const;
			uint32 getLastExpressionReferenceIndex() const;

			bool read(TokenType type) const;

			Token read() const;
			Token peek() const;

			void jump();
			void jumpEol();
			void synchronize();

			// Registers a semantic token for a variable/constant/member/parameter.
			// If false is returned, it means the parser needs more context to register the token.
			bool registerVariableSemanticToken(const Token& token, bool preferStructure) const;

		public:
			CompilationOutput* output;
			uint32 tokenIndex;
			uint32 currentAnnotationCount;
			uint8 annotationFlag;
			DocumentPosition position;
			uint32 visitedWhileCount;

			SemanticAnalyzer analyzer;
	};
}