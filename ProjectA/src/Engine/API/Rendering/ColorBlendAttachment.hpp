#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace ProjectA {
	struct ColorBlendAttachment {
		bool blend;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
	};
}