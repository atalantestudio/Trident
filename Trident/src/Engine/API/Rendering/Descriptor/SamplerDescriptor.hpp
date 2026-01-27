#pragma once

#include "Engine/API/Rendering/SamplerFilter.hpp"

namespace Trident {
	struct SamplerDescriptor {
		SamplerFilter magFilter;
		SamplerFilter minFilter;
	};
}