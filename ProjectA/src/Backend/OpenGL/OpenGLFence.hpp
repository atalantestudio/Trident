#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Synchronization/Fence.hpp"

namespace ProjectA {
	class OpenGLFence : public Fence, public ObjectWrapper<GLsync> {
		public:
			void create();

			void reset();
	};
}