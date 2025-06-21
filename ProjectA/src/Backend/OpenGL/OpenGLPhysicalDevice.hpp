#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace ProjectA {
	class OpenGLPhysicalDevice final : public PhysicalDevice {
		public:
			explicit OpenGLPhysicalDevice();

			void setLimits();
	};
}