#pragma once

#include "../DocumentRange.hpp"

namespace ProjectA {
	struct DiagnosticReference {
		DocumentRange range;
		std::string message;
	};
}