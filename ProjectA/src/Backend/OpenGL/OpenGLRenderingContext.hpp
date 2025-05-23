#pragma once

namespace ProjectA {
	#define LOAD_PROCEDURE_ADDRESS(renderingContext, procedure) procedure = static_cast<decltype(procedure)>(renderingContext.getProcedureAddress(#procedure))

	class OpenGLRenderingContext {
		public:
			static void* getProcedureAddress(std::string_view name);

		public:
			OpenGLRenderingContext(const Window& window);

			~OpenGLRenderingContext();

			void makeCurrent() const;

			void setSwapInterval(sint32 interval) const;

			void swapBuffers() const;

		private:
			HDC hDC;
			HGLRC hGLRC;
	};
}