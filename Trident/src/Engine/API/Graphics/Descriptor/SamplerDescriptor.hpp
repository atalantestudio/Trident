// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Filter.hpp"

namespace Trident {
	struct SamplerDescriptor {
		/// The magnification filter used for texture lookups.
		Filter magFilter;

		/// The minification filter used for texture lookups.
		Filter minFilter;
	};
}