// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Surface.hpp"

namespace Trident {
	class GL::Surface final : public Trident::Surface {
		public:
			ImageFormat getFormat() const final override;
	};
}