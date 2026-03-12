// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class CommandBufferUsage : uint8 {
		AUTO,
		ONE_TIME_SUBMIT,

		count,
	};
}