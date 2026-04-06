// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ImageLayout : uint8 {
		UNDEFINED,
		GENERAL,
		COLOR_ATTACHMENT_OPTIMAL,
		DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		SHADER_READ_ONLY_OPTIMAL,
		TRANSFER_SRC_OPTIMAL,
		TRANSFER_DST_OPTIMAL,
		PRESENT_SRC,

		count,
	};
}