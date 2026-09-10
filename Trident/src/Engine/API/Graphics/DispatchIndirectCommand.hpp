// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct DispatchIndirectCommand {
		/// The number of local work groups to dispatch in the X dimension.
		uint32 x;

		/// The number of local work groups to dispatch in the Y dimension.
		uint32 y;

		/// The number of local work groups to dispatch in the Z dimension.
		uint32 z;
	};

	ENGINE_STATIC_ASSERT(sizeof(DispatchIndirectCommand) == 12);
}