// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Validator {
		/// Triggers a breakpoint if the current scope was marked as invalid.
		~Validator() {
			ENGINE_ASSERT(errorCount == 0, "[] validation errors detected.", errorCount);
		}

		uint32 errorCount = 0;
	};
}