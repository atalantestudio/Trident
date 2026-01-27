// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDeviceLimits.hpp"

namespace Trident {
	class PhysicalDevice {
		public:
			virtual ~PhysicalDevice() = default;

			const PhysicalDeviceLimits& getLimits() const {
				return limits;
			}

		protected:
			PhysicalDeviceLimits limits;
	};
}