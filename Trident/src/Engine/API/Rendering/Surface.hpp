// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"

namespace Trident {
	class Surface {
		public:
			virtual ~Surface() = default;

			virtual ImageFormat getFormat() const = 0;
	};
}