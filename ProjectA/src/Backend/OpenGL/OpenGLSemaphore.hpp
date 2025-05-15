#pragma once

#include "Engine/API/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class OpenGLSemaphore : public Semaphore {
		public:
			OpenGLSemaphore() = default;
	};
}