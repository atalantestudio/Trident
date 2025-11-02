#pragma once

#include "Annotation.hpp"

namespace ProjectA {
	struct AnnotationBlock {
		Annotation annotation = static_cast<Annotation>(~0);
		uint32 identifierTokenIndex = 0;
		uint32 valueTokenIndex = static_cast<uint32>(~0);
	};
}