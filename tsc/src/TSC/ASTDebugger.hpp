#pragma once

#include "Engine/Logger/FileLogger/JSONFileLogger.hpp"

#include "TSC/CompilationOutput.hpp"

namespace ProjectA {
	class ASTDebugger {
		public:
			explicit ASTDebugger(std::string_view path, const CompilationOutput& output);

		private:
			void writeStatement(uint32 statementIndex);
			void writeStructureStatement(uint32 statementIndex);
			void writeMemberStatement(uint32 statementIndex);
			void writeConstantStatement(uint32 statementIndex);
			void writeVariableStatement(uint32 statementIndex);
			void writeParameterStatement(uint32 statementIndex);
			void writeFunctionStatement(uint32 statementIndex);
			void writeExpressionStatement(uint32 statementIndex);
			void writeScopeStatement(uint32 statementIndex);
			void writeIfStatement(uint32 statementIndex);
			void writeWhileStatement(uint32 statementIndex);
			void writeBreakStatement();
			void writeNextStatement();
			void writeReturnStatement(uint32 statementIndex);

			void writeExpression(uint32 referenceIndex);
			void writeLiteralExpression(uint32 byteOffset);
			void writeIdentifierExpression(uint32 byteOffset);
			void writeUnaryExpression(uint32 byteOffset);
			void writeBinaryExpression(uint32 byteOffset);
			void writeGroupExpression(uint32 byteOffset);
			void writeMemberAccessExpression(uint32 byteOffset);
			void writeCallExpression(uint32 byteOffset);
			void writeTypeExpression(uint32 byteOffset);

			void writeAnnotationBlocks(uint32 indexOffset, uint32 count);

		private:
			FileLogger<FileFormat::JSON> logger;
			const CompilationOutput* output;
	};
}