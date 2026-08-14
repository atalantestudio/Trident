// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Window/LayerGroup/LayerGroup.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	LayerGroup::LayerGroup(range<uint32, uint32> layerRange) :
		layerRange(layerRange)
	{}

	void LayerGroup::setApplication(Application* application) {
		this->application = application;

		layers = view<ref<Layer>>(&application->getLayerCompositor()->getLayers()[layerRange.offset], layerRange.count);
	}

	void LayerGroup::initialize() {}

	void LayerGroup::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		for (ref<Layer> layer : layers) {
			layer->preRender();
			layer->render(commandEncoder, remainingFrameTime);
			layer->postRender();
		}
	}

	void LayerGroup::resize() {}
}