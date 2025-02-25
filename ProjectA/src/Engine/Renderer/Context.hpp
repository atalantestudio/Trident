#pragma once

#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class Context {
		public:
			static scope<Context> create(void* window);

		public:
			virtual ~Context() = default;

			virtual void swapBuffers() const = 0;
	};
}