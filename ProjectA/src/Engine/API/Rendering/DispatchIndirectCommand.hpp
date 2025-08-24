#pragma once

namespace ProjectA {
	struct DispatchIndirectCommand {
		// The number of local workgroups to dispatch in the X dimension.
		uint32 x;

		// The number of local workgroups to dispatch in the Y dimension.
		uint32 y;

		// The number of local workgroups to dispatch in the Z dimension.
		uint32 z;
	};
}