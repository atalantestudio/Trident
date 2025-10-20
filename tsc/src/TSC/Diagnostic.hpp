#pragma once

#include "DiagnosticSeverity.hpp"
#include "Position.hpp"

namespace ProjectA {
	class Diagnostic {
		public:
			DiagnosticSeverity severity;
			std::string message;
			Position start;
			Position end;
	};
}