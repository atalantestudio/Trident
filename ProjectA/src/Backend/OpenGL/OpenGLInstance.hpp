#pragma once

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	typedef WINGDIAPI HGLRC (WINAPI *PFNWGLCREATECONTEXTPROC)(HDC);
	typedef WINGDIAPI BOOL (WINAPI *PFNWGLDELETECONTEXTPROC)(HGLRC);
	typedef WINGDIAPI PROC (WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR);
	typedef WINGDIAPI BOOL (WINAPI *PFNWGLMAKECURRENTPROC)(HDC, HGLRC);

	class OpenGLInstance {
		private:
			static void* getWglProcedure(view<char8> procedureName);

		public:
			explicit OpenGLInstance(const Window& window);

			~OpenGLInstance();

			void* getProcedure(view<char8> procedureName) const;

			void makeCurrent() const;

			void setSwapInterval(sint32 interval) const;

			void swapBuffers() const;

		private:
			HDC hDC;
			HGLRC hGLRC;
			PFNWGLMAKECURRENTPROC wglMakeCurrent;
			PFNWGLDELETECONTEXTPROC wglDeleteContext;
			PFNWGLGETPROCADDRESSPROC wglGetProcAddress;
	};
}