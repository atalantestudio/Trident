#pragma once

#include "AnnotationInstance.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ProjectA {
	struct AST {
		std::vector<ref<Statement>> statements;
		std::vector<ref<Expression>> expressions;
		std::vector<AnnotationInstance> annotationInstances;
		std::vector<std::string> userDefinedTypes;
		std::vector<uint32> globalScopeStatementIndices;
	};
}