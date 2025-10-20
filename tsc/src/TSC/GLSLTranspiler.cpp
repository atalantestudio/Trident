#include "pch.hpp"

#include "GLSLTranspiler.hpp"

namespace ProjectA {
	void GLSLTranspiler::write(const char8* path, const CompilationResult& result, const AST& ast) {
		stream.open(path);

		DEBUG_ASSERT(stream.is_open());

		this->result = &result;
		this->ast = &ast;

		stream << "#version 310 es";

		for (const uint32 globalScopeStatementIndex : this->ast->globalScopeStatementIndices) {
			stream << "\n\n";

			writeStatement(globalScopeStatementIndex);
		}

		stream.close();
	}

	void GLSLTranspiler::writeStatement(uint32 globalScopeStatementIndex) {
		const ref<Statement> statement = ast->statements[globalScopeStatementIndex];

		switch (statement->statementType) {
			case StatementType::STRUCTURE:
				return writeStructureStatement(std::static_pointer_cast<StructureStatement>(statement));
			default:
				// TODO
				break;
		}
	}

	void GLSLTranspiler::writeStructureStatement(const ref<StructureStatement> statement) {
		const Token& identifierToken = result->tokens[statement->identifierTokenIndex];

		stream << indent() << "struct _" << getTokenString(identifierToken) << " {\n";

		_indent += 1;

		for (const uint32 memberStatementIndex : statement->memberStatementIndices) {
			const ref<VariableStatement> memberStatement = std::static_pointer_cast<VariableStatement>(ast->statements[memberStatementIndex]);

			writeMemberStatement(memberStatement);
		}

		_indent -= 1;

		stream << "};";
	}

	void GLSLTranspiler::writeMemberStatement(const ref<VariableStatement> memberStatement) {
		if (memberStatement->annotationFlag & (Annotation::INSTANCE_INDEX | Annotation::INSTANCE_INDEX | Annotation::VERTEX_INDEX)) {
			return;
		}

		stream << indent();

		if (memberStatement->annotationFlag & Annotation::LOCATION) {
			stream << "layout(";

			const AnnotationInstance* locationAnnotationInstance = nullptr;

			for (uint32 annotationInstanceIndex = 0; annotationInstanceIndex < memberStatement->annotationInstanceCount; annotationInstanceIndex += 1) {
				locationAnnotationInstance = &ast->annotationInstances[memberStatement->annotationInstanceIndexOffset + annotationInstanceIndex];

				if (locationAnnotationInstance->annotation == Annotation::LOCATION) {
					break;
				}
			}

			stream << "location = " << locationAnnotationInstance->value.value << ") ";
		}

		const ref<Expression>& typeExpression = ast->expressions[memberStatement->typeExpressionIndex];

		writeTypeExpression(std::static_pointer_cast<TypeExpression>(typeExpression));

		const Token& identifierToken = result->tokens[memberStatement->identifierTokenIndex];

		stream << " _" << getTokenString(identifierToken) << ";\n";
	}

	void GLSLTranspiler::writeTypeExpression(const ref<TypeExpression> expression) {
		stream << getGlslType(expression->typeTokenIndex, expression->templateArgumentTokenIndex);
	}
}