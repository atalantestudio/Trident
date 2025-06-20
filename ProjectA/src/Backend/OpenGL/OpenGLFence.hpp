#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class OpenGLFence final : public Fence, public ObjectWrapper<GLsync> {
		public:
			OpenGLFence();

			void create();

			void reset();
	};
}