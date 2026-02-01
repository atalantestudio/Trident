// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/MaterialDescriptor.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"

namespace Trident {
	class Material {
		public:
			explicit Material(const MaterialDescriptor& descriptor) :
				writeDescriptorSets(descriptor.writeDescriptorSets.count())
			{}

		protected:
			sequence<WriteDescriptorSet> writeDescriptorSets;

		private:
			uint32 pipelineIndex;
	};
}