#pragma once

#include "Annotation.hpp"

namespace ProjectA {
	static constexpr Annotation UNDEFINED_ANNOTATION = static_cast<Annotation>(~0);
	static constexpr uint32 UNDEFINED_ANNOTATION_VALUE = ~0;

	struct AnnotationBlock {
		Annotation annotation = UNDEFINED_ANNOTATION;
		uint32 identifierTokenIndex = 0;
		uint32 valueTokenIndex = UNDEFINED_ANNOTATION_VALUE;
	};
}