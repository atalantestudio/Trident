// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	void GI2DExperimentalRenderer::updateDescriptorSets() const {
		const WriteDescriptorSet writeDescriptorSets[] = {
			// Vertex
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 0,
				.descriptorBufferViews = vertexBufferView,
			},

			// Camera
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.descriptorBufferViews = cameraBufferView,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}
}