// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/MemoryProperty.hpp"

namespace Trident {
	struct BufferDescriptor {
		// Size of the buffer in bytes.
		uint64 size;

		BufferUsage usage;
		MemoryProperty memoryProperties;

		// Indicates if the buffer is going to be mapped for the application's lifetime.
		// Only used by OpenGL.
		bool persistent = false;
	};
}