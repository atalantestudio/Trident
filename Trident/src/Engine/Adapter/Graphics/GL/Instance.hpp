// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	typedef WINGDIAPI HGLRC (WINAPI *PFNWGLCREATECONTEXTPROC)(HDC);
	typedef WINGDIAPI BOOL (WINAPI *PFNWGLDELETECONTEXTPROC)(HGLRC);
	typedef WINGDIAPI PROC (WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR);
	typedef WINGDIAPI BOOL (WINAPI *PFNWGLMAKECURRENTPROC)(HDC, HGLRC);

	class GL::Instance final : public Trident::Instance {
		private:
			static void* getWglProcedure(view<char8> procedureName);

		public:
			explicit Instance(const Window& window);

			~Instance() final override;

			void* getProcedure(view<char8> procedureName) const;

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