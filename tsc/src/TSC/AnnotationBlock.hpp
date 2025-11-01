#pragma once

#include "Annotation.hpp"

// TODO: Move UNDEFINED_TOKEN to a base file.
#include "Expression.hpp"

namespace ProjectA {
	static constexpr Annotation UNDEFINED_ANNOTATION = static_cast<Annotation>(~0);

	struct AnnotationBlock {
		Annotation annotation = UNDEFINED_ANNOTATION;
		uint32 identifierTokenIndex = 0;
		uint32 valueTokenIndex = UNDEFINED_TOKEN;
	};
}