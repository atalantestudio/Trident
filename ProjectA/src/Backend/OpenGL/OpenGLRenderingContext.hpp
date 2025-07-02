#pragma once

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class OpenGLRenderingContext {
		public:
			static void* getProcedureAddress(std::string_view name);

		public:
			explicit OpenGLRenderingContext(const Window& window);

			~OpenGLRenderingContext();

			void makeCurrent() const;

			void setSwapInterval(sint32 interval) const;

			void swapBuffers() const;

		private:
			HDC hDC;
			HGLRC hGLRC;
	};
}