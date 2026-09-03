// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	class RayTracingPipeline {
		public:
			~RayTracingPipeline();

		public:
			ref<Device> device;
	};
}