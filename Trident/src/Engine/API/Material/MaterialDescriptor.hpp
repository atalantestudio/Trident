// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"

namespace Trident {
	struct MaterialDescriptor {
		uint32 pipelineIndex;
		view<WriteDescriptorSet> writeDescriptorSets;
	};
}