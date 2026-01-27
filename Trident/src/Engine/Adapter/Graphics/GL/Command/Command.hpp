// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// Used for queuing graphics and compute operations to the GPU.
	class GL::Command {
		public:
			virtual void execute() const = 0;
	};
}