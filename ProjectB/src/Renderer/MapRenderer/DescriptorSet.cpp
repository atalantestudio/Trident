// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	void MapRenderer::updateDescriptorSets() const {
		sequence<WriteDescriptorSet> writeDescriptorSets(staticGeometryDescriptorSets.count());
		uint32 descriptorSetIndex = 0;

		while (descriptorSetIndex < staticGeometryDescriptorSets.count()) {
			const ref<DescriptorSet> descriptorSet = staticGeometryDescriptorSets[descriptorSetIndex];

			WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[descriptorSetIndex];
			writeDescriptorSet.descriptorSet = descriptorSet;
			writeDescriptorSet.bindingIndex = 0;
			writeDescriptorSet.descriptorBufferViews = view<BaseBufferView>(pointer_cast<const BaseBufferView*>(&cameraViews[descriptorSetIndex]), 1);

			descriptorSetIndex += 1;
		}

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}
}