// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct FenceDescriptor {
		// Whether the fence is created in the signaled or unsignaled state.
		bool signaled;
	};
}