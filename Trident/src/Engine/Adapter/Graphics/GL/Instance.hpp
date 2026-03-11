// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	typedef HGLRC (*PFNWGLCREATECONTEXTPROC)(HDC);
	typedef BOOL (*PFNWGLDELETECONTEXTPROC)(HGLRC);
	typedef PROC (*PFNWGLGETPROCADDRESSPROC)(LPCSTR);
	typedef BOOL (*PFNWGLMAKECURRENTPROC)(HDC, HGLRC);

	class GL::Instance final : public Trident::Instance {
		private:
			static void* getWglProcedure(view<char8> procedureName);

		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter);

			void* getProcedure(view<char8> procedureName) const;

		public:
			HDC deviceContext;
			HGLRC glRenderingContext;
			PFNWGLMAKECURRENTPROC wglMakeCurrent;
			PFNWGLDELETECONTEXTPROC wglDeleteContext;
			PFNWGLGETPROCADDRESSPROC wglGetProcAddress;
	};

	namespace GL {
		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		void destroyInstance(const Trident::Instance* instance);
	}
}