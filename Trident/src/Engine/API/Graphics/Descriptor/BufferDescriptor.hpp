// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferUsage.hpp"
#include "Engine/API/Graphics/MemoryProperty.hpp"

namespace Trident {
	struct BufferDescriptor {
		/// Size in bytes of the buffer.
		uint64 size;

		BufferUsage usage;
		MemoryProperty memoryProperties;

		// Indicates if the buffer is going to be mapped for the application's lifetime.
		// Only used by OpenGL.
		bool persistent = false;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}