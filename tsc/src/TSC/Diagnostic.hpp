#pragma once

#include "DiagnosticSeverity.hpp"
#include "Position.hpp"

namespace ProjectA {
	class DiagnosticRelatedInformation {
		public:
			uint32 getStartLineIndex() const {
				return start.lineIndex;
			}

			uint32 getStartCharacterOffset() const {
				return start.characterOffset;
			}

			uint32 getEndLineIndex() const {
				return start.lineIndex;
			}

			uint32 getEndCharacterOffset() const {
				return start.characterOffset;
			}

			const std::string& getMessage() const {
				return message;
			}

		public:
			Position start;
			Position end;
			std::string message;
	};

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

			const std::vector<DiagnosticRelatedInformation>& getRelatedInformations() const {
				return relatedInformations;
			}

		public:
			DiagnosticSeverity severity;
			std::string message;
			Position start;
			Position end;
			std::vector<DiagnosticRelatedInformation> relatedInformations;
	};
}