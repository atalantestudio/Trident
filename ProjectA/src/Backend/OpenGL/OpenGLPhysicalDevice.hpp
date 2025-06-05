#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace ProjectA {
	class OpenGLPhysicalDevice : public PhysicalDevice {
		public:
			OpenGLPhysicalDevice();

			void setLimits();
	};
}