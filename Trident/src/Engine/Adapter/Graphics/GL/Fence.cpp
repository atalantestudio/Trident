// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Fence::Fence(const FenceDescriptor& descriptor) :
		Trident::Fence(descriptor),
		sync(nullptr)
	{
		if (!descriptor.signaled) {
			create();
		}
	}

	void GL::Fence::create() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			// See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml.
			sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

			ADAPTER_ASSERT(sync != 0, "Failed to create fence.");
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void GL::Fence::reset() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glDeleteSync(sync);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		sync = nullptr;
	}
}