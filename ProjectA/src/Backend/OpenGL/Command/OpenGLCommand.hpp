#pragma once

namespace ProjectA {
	// Used for queuing graphics and compute operations to the GPU.
	class OpenGLCommand {
		public:
			virtual void execute() const = 0;
	};
}