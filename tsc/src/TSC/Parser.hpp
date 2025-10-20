#pragma once

#include "AST.hpp"
#include "CompilationResult.hpp"
#include "Position.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace ProjectA {
	class Parser {
		public:
			AST parse(CompilationResult& compilationResult);

		private:
			bool parseGlobalScopeStatement();
			bool parseStructureStatement();

			// Returns a variable statement which is guaranteed to have an initializer.
			bool parseConstantOrFunctionStatement();

			bool parseFunctionStatement(const ref<VariableStatement>& constantStatement);

			// Returns a variable statement which is guaranteed to not have an initializer.
			bool parseMemberOrParameterStatement();

			bool parseLocalScopeStatement();
			bool parseScopeStatement();
			bool parseIfStatementStart();
			bool parseReturnStatement();
			bool parseWhileStatementStart();
			bool parseExpressionStatement();
			bool parseIfOrWhileStatementEnd(uint32& conditionExpressionIndex, uint32& scopeStatementIndex);

			// Can parse a variable or member access statement.
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

			Position getPosition() const;

			Token read() const;
			Token peek() const;

			void jump();
			void skipEmptyLines();
			void synchronize();

		public:
			CompilationResult* compilationResult;
			AST ast;
			uint32 tokenIndex = 0;
			Position position;
	};
}