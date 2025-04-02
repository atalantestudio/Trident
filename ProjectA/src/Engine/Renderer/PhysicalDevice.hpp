#pragma once

#include "Engine/Renderer/PhysicalDeviceLimits.hpp"

namespace ProjectA {
	class PhysicalDevice {
		public:
			~PhysicalDevice() = default;

			virtual const PhysicalDeviceLimits& getLimits() const = 0;

		protected:
			PhysicalDeviceLimits limits;
	};
}