#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace Trident {
	struct ColorBlendAttachment {
		bool blend;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
	};
}