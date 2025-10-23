#pragma once

#include "CompilationOutput.hpp"
#include "DocumentPosition.hpp"
#include "SemanticAnalyzer.hpp"
#include "Token.hpp"

namespace ProjectA {
	class Parser {
		public:
			void parse(CompilationOutput* _output);

		private:
			bool parseGlobalScopeStatement();
			bool parseStructureStatement();
			bool parseMemberStatement();
			bool parseConstantOrFunctionStatement();
			bool parseFunctionStatement();
			bool parseParameterStatement();

			bool parseLocalScopeStatement();
			bool parseScopeStatement();
			bool parseIfStatementStart();
			bool parseReturnStatement();
			bool parseBreakStatement();
			bool parseNextStatement();
			bool parseWhileStatementStart();
			bool parseExpressionStatement();
			bool parseIfOrWhileStatementEnd(uint32& conditionExpressionIndex, uint32& scopeStatementIndex);
			bool parseVariableStatement();

			bool parseTypeExpression();

			bool parseAssignmentOrCompoundExpression();
			bool parseLogicalOrExpression();
			bool parseLogicalXorExpression();
			bool parseLogicalAndExpression();
			bool parseBitwiseOrExpression();
			bool parseBitwiseXorExpression();
			bool parseBitwiseAndExpression();
			bool parseEqualityExpression();
			bool parseRelationalExpression();
			bool parseBitwiseShiftExpression();
			bool parseAdditiveExpression();
			bool parseMultiplicativeExpression();
			bool parseUnaryExpression();
			bool parsePrimaryExpression();

			bool parseIdentifierExpression();
			bool parseMemberAccessExpression();
			bool parseFunctionCallOrGroupExpression();
			bool parseNumberExpression();
			bool parseGroupExpression();

			bool parseAnnotationInstances();

			uint32 getLastStatementIndex() const;
			uint32 getLastExpressionIndex() const;

			Token read() const;
			Token peek() const;

			void jump();
			void skipEmptyLines();
			void synchronize();

			// Registers a semantic token for a variable/constant/member/parameter.
			// If false is returned, it means the parser needs more context to register the token.
			bool registerVariableSemanticToken(const Token& token, bool preferStructure = false) const;

		public:
			CompilationOutput* output;
			uint32 tokenIndex;
			uint32 currentAnnotationCount;
			uint8 annotationFlag;
			DocumentPosition position;

			uint32 parentStatementIndex;
			uint32 visitedWhileCount;

			SemanticAnalyzer analyzer;
	};
}