#pragma once

namespace ProjectA {
	class Context {
		public:
			static Scope<Context> create(void* window);

		public:
			virtual ~Context() = default;

			virtual void initialize() = 0;

			virtual void swapBuffers() = 0;
	};
}