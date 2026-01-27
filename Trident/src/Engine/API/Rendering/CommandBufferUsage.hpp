// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class CommandBufferUsage : uint8 {
		AUTO = 0,
		ONE_TIME_SUBMIT = BIT(0),
		SIMULTANEOUS_USE = BIT(2),
	};

	AS_FLAG(CommandBufferUsage);
}