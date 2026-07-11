// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/AddressMode.hpp"
#include "Engine/API/Graphics/Filter.hpp"

namespace Trident {
	struct SamplerDescriptor {
		/// The magnification filter used for texture lookups.
		Filter magFilter;

		/// The minification filter used for texture lookups.
		Filter minFilter;

		/// Wrapping operation for the U texture coordinate.
		AddressMode addressModeU;

		/// Wrapping operation for the V texture coordinate.
		AddressMode addressModeV;

		/// Wrapping operation for the W texture coordinate.
		AddressMode addressModeW;
	};
}