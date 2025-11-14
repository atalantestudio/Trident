#pragma once

#include "Engine/Filesystem/FileWriter/JSONFileWriter.hpp"

#include "TSC/CompilationOutput.hpp"

namespace ProjectA {
	class ASTDebugger {
		public:
			explicit ASTDebugger(view<char8> path, const CompilationOutput& output);

		private:
			void writeStatement();
			void writeStructureStatement(const Statement<StatementType::STRUCTURE>& statement);
			void writeMemberStatement(const Statement<StatementType::MEMBER>& statement);
			void writeConstantStatement(const Statement<StatementType::CONSTANT>& statement);
			void writeVariableStatement(const Statement<StatementType::VARIABLE>& statement);
			void writeParameterStatement(const Statement<StatementType::PARAMETER>& statement);
			void writeFunctionStatement(const Statement<StatementType::FUNCTION>& statement);
			void writeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement);
			void writeScopeStatement(const Statement<StatementType::SCOPE>& statement);
			void writeIfStatement(const Statement<StatementType::IF>& statement);
			void writeWhileStatement(const Statement<StatementType::WHILE>& statement);
			void writeBreakStatement(const Statement<StatementType::BREAK>& statement);
			void writeNextStatement(const Statement<StatementType::NEXT>& statement);
			void writeReturnStatement(const Statement<StatementType::RETURN>& statement);

			void writeExpression();
			void writeLiteralExpression();
			void writeIdentifierExpression();
			void writeUnaryExpression();
			void writeBinaryExpression();
			void writeGroupExpression();
			void writeMemberAccessExpression();
			void writeCallExpression();
			void writeTypeExpression();

			void writeAnnotationBlocks(uint32 indexOffset, uint32 count);

		private:
			FileWriter<FileFormat::JSON> writer;
			const CompilationOutput* output;

			uint32 statementIndex;
			uint32 statementByteOffset;
			uint32 expressionReferenceIndex;
			uint32 expressionByteOffset;
	};
}