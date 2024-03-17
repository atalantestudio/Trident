#pragma once

namespace ProjectA {
	class Context {
		public:
			virtual void initialize() = 0;

			virtual void swapBuffers() = 0;
	};
}