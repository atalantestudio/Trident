// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class Access : uint8 {
		NONE = 0,
		SHADER_READ = BIT(0),
		SHADER_WRITE = BIT(1),
		TRANSFER_READ = BIT(2),
		TRANSFER_WRITE = BIT(3),
	};

	AS_FLAG(Access);
}