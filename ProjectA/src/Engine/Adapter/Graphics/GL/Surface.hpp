// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Surface.hpp"

namespace ProjectA {
	class GL::Surface final : public ProjectA::Surface {
		public:
			ImageFormat getFormat() const final override;
	};
}