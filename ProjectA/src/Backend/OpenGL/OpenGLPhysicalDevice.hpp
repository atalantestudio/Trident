#pragma once

#include "Engine/Renderer/PhysicalDevice.hpp"

namespace ProjectA {
	class OpenGLPhysicalDevice : public PhysicalDevice {
		public:
			OpenGLPhysicalDevice();

			void setLimits();
	};
}