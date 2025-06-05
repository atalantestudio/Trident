#pragma once

#include "Engine/API/Rendering/PhysicalDeviceLimits.hpp"

namespace ProjectA {
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