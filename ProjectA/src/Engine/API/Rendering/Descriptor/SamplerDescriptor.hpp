#pragma once

#include "Engine/API/Rendering/SamplerFilter.hpp"

namespace ProjectA {
	struct SamplerDescriptor {
		SamplerFilter magFilter;
		SamplerFilter minFilter;
	};
}