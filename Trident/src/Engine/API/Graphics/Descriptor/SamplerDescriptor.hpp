// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/SamplerFilter.hpp"

namespace Trident {
	struct SamplerDescriptor {
		SamplerFilter magFilter;
		SamplerFilter minFilter;
	};
}