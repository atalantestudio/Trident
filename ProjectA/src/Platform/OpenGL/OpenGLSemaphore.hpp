#pragma once

#include "Engine/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class OpenGLSemaphore : public Semaphore {
		public:
			OpenGLSemaphore();
	};
}