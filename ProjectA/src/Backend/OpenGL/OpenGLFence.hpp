#pragma once

#include "Engine/Synchronization/Fence.hpp"

namespace ProjectA {
	class OpenGLFence : public Fence {
		public:
			OpenGLFence();
	};
}