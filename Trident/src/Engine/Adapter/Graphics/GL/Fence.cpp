// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Fence::Fence(const Trident::Device* device) :
		Trident::Fence(device)
	{}

	ref<Trident::Fence> GL::createFence(const Trident::Device* device, const FenceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Fence> fence = allocateRef<GL::Fence>(device);

		// TODO(graphics/gl/fence): Create sync object here?
		/* if (!descriptor.signaled) {
			#if OPENGL_VERSION >= OPENGL_VERSION_3_2
				fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

				ADAPTER_ASSERT(fence->sync != 0, "Failed to create fence.");
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		} */

		return fence;
	}

	void GL::destroyFence(const Trident::Device* device, const Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Fence* _fence = static_cast<const GL::Fence*>(fence);

		if (_fence->sync) {
			#if OPENGL_VERSION >= OPENGL_VERSION_3_2
				glDeleteSync(_fence->sync);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		}
	}
}