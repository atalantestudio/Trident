// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	struct RayTracingPipelineDescriptor {
		ref<PipelineLayout> layout;
		view<ref<ShaderModule>> shaderModules;
		uint32 maxRayRecursionDepth;

		/// The maximum payload size in bytes used by any shader in the pipeline.
		uint32 maxRayPayloadSize;

		/// The maximum attribute size in bytes used by any shader in the pipeline.
		uint32 maxRayAttributeSize;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}