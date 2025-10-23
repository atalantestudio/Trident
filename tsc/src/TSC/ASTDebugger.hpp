#pragma once

#include "TSC/CompilationOutput.hpp"

namespace ProjectA {
	class ASTDebugger {
		public:
			void write(std::ofstream& stream, const CompilationOutput& output);

		private:
			void writeStatement(std::ofstream& stream, uint32 statementIndex);
			void writeStructureStatement(std::ofstream& stream, uint32 statementIndex);
			void writeMemberStatement(std::ofstream& stream, uint32 statementIndex);
			void writeConstantStatement(std::ofstream& stream, uint32 statementIndex);
			void writeVariableStatement(std::ofstream& stream, uint32 statementIndex);
			void writeParameterStatement(std::ofstream& stream, uint32 statementIndex);
			void writeFunctionStatement(std::ofstream& stream, uint32 statementIndex);
			void writeExpressionStatement(std::ofstream& stream, uint32 statementIndex);
			void writeScopeStatement(std::ofstream& stream, uint32 statementIndex);
			void writeIfStatement(std::ofstream& stream, uint32 statementIndex);
			void writeWhileStatement(std::ofstream& stream, uint32 statementIndex);
			void writeBreakStatement(std::ofstream& stream) const;
			void writeNextStatement(std::ofstream& stream) const;
			void writeReturnStatement(std::ofstream& stream, uint32 statementIndex);

			void writeExpression(std::ofstream& stream, uint32 referenceIndex);
			void writeNumberExpression(std::ofstream& stream, uint32 byteOffset) const;
			void writeIdentifierExpression(std::ofstream& stream, uint32 byteOffset) const;
			void writeUnaryExpression(std::ofstream& stream, uint32 byteOffset);
			void writeBinaryExpression(std::ofstream& stream, uint32 byteOffset);
			void writeGroupExpression(std::ofstream& stream, uint32 byteOffset);
			void writeMemberAccessExpression(std::ofstream& stream, uint32 byteOffset);
			void writeFunctionCallExpression(std::ofstream& stream, uint32 byteOffset);
			void writeTypeExpression(std::ofstream& stream, uint32 byteOffset) const;

			void writeAnnotationBlocks(std::ofstream& stream, uint32 indexOffset, uint32 count);

			void indent();
			void unindent();

		private:
			const CompilationOutput* output;

			uint32 indentationLevel;
			std::string indentation;
	};
}