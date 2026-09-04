// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	class RayTracingPipeline {
		public:
			~RayTracingPipeline();

			/// Queries a range of the shader group handles used by this pipeline.
			/// The handles are written to `mappedRange`.
			void getShaderGroupHandles(range<uint32, uint32> groupRange, void* mappedRange) const;

		public:
			ref<Device> device;
	};
}