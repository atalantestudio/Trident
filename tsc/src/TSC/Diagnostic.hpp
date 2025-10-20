#pragma once

#include "DiagnosticSeverity.hpp"
#include "Position.hpp"

namespace ProjectA {
	// TODO: Find a way to serialize without having to write getters.
	class Diagnostic {
		public:
			uint32 getSeverityIndex() const {
				return static_cast<uint32>(severity);
			}

			// TODO: See if possible to return std::string_view.
			std::string getMessage() const {
				return message;
			}

			uint32 getStartLineIndex() const {
				return start.lineIndex;
			}

			uint32 getStartCharacterOffset() const {
				return start.characterOffset;
			}

			uint32 getEndLineIndex() const {
				return end.lineIndex;
			}

			uint32 getEndCharacterOffset() const {
				return end.characterOffset;
			}

		public:
			DiagnosticSeverity severity;
			std::string message;
			Position start;
			Position end;
	};
}