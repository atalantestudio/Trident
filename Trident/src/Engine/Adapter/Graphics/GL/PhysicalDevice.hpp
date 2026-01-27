// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class GL::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice();

			void setLimits();
	};
}