// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace Trident {
	struct ColorBlendAttachment {
		bool blend;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
	};
}