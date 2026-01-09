// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace ProjectA {
	class GL::PhysicalDevice final : public ProjectA::PhysicalDevice {
		public:
			explicit PhysicalDevice();

			void setLimits();
	};
}