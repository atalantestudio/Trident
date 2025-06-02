#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Synchronization/Fence.hpp"

namespace ProjectA {
	class OpenGLFence : public Fence, public ObjectWrapper<GLsync> {
		public:
			OpenGLFence();

			void create();

			void reset();

			bool isSignaled() const final override;
	};
}