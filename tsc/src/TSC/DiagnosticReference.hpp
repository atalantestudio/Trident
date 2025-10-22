#pragma once

#include "Position.hpp"

namespace ProjectA {
	class DiagnosticReference {
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
}