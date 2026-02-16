// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class Access : uint8 {
		SHADER_READ = BIT(0),
		SHADER_WRITE = BIT(1),
	};

	AS_FLAG(Access);
}