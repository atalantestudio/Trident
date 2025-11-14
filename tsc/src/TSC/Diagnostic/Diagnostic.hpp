#pragma once

#include "DiagnosticCode.hpp"
#include "DiagnosticReference.hpp"
#include "DiagnosticSeverity.hpp"

namespace ProjectA {
	class Diagnostic {
		public:
			uint32 getSeverityIndex() const {
				return static_cast<uint32>(severity);
			}

			uint32 getCodeIndex() const {
				return static_cast<uint32>(code);
			}

			std::string getMessage() const {
				return std::string(&message[0], message.count());
			}

		public:
			DocumentRange range;
			sequence<char8> message;
			DiagnosticSeverity severity;
			DiagnosticCode code;
			std::vector<DiagnosticReference> references;
	};
}