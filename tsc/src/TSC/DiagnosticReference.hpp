#pragma once

#include "DocumentPosition.hpp"

namespace ProjectA {
	struct DiagnosticReference {
		DocumentPosition start;
		DocumentPosition end;
		std::string message;
	};
}