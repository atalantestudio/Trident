#pragma once

#include "CompilationOutput.hpp"
#include "Diagnostic/DiagnosticCode.hpp"
#include "DocumentPosition.hpp"
#include "Token.hpp"

namespace ProjectA {
	class Parser {
		public:
			void parse(CompilationOutput* output);

		private:
			DiagnosticCode parseGlobalScopeStatement();
			DiagnosticCode parseStructureStatement();
			DiagnosticCode parseMemberStatement();
			DiagnosticCode parseConstantOrFunctionStatement();
			DiagnosticCode parseFunctionStatement();
			DiagnosticCode parseParameterStatement();

			DiagnosticCode parseLocalScopeStatement();
			DiagnosticCode parseScopeStatement();
			DiagnosticCode parseIfStatement();
			DiagnosticCode parseReturnStatement();
			DiagnosticCode parseBreakStatement();
			DiagnosticCode parseNextStatement();
			DiagnosticCode parseWhileStatement();
			DiagnosticCode parseExpressionStatement();
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
			DiagnosticCode parseCallOrGroupExpression(uint32 insertionIndex, bool explicitCallee);
			DiagnosticCode parseLiteralExpression();
			DiagnosticCode parseGroupExpression();

			DiagnosticCode parseAnnotationBlocks();

			Token read() const;
			Token peek() const;

			bool read(TokenType type) const;

			void jump();
			void jumpEol();
			void synchronize();

		public:
			CompilationOutput* output;
			uint32 tokenIndex;
			uint32 currentAnnotationCount;
			uint8 annotationFlag;
			DocumentPosition position;
			uint32 visitedWhileCount;
	};
}