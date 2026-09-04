// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	struct RayTracingPipelineDescriptor {
		ref<PipelineLayout> layout;
		view<ref<ShaderModule>> shaderModules;
		uint32 maxRecursionDepth;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}