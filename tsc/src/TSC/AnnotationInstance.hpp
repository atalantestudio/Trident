#pragma once

#include "Annotation.hpp"
#include "Token.hpp"

namespace ProjectA {
	struct AnnotationInstance {
		Annotation annotation = static_cast<Annotation>(0);
		Token identifier;
		Token value;
	};
}