#include "pch.hpp"
#include "LayerStack.hpp"

namespace ProjectA {
	LayerStack::LayerStack() {}

	LayerStack::~LayerStack() {
		for (Layer* layer : layers) {
			layer->unbind();

			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer) {
		layers.emplace(layers.begin() + layerInsertionIndex, layer);

		layerInsertionIndex++;
	}

	void LayerStack::pushOverlay(Layer* overlay) {
		layers.emplace_back(overlay);
	}

	void LayerStack::removeLayer(Layer* layer) {
		auto iterator = std::find(layers.begin(), layers.begin() + layerInsertionIndex, layer);

		if (iterator == layers.begin() + layerInsertionIndex) {
			return;
		}

		layers.erase(iterator);

		layerInsertionIndex--;

		layer->unbind();
	}

	void LayerStack::removeOverlay(Layer* overlay) {
		auto iterator = std::find(layers.begin() + layerInsertionIndex, layers.end(), overlay);

		if (iterator == layers.end()) {
			return;
		}

		layers.erase(iterator);

		overlay->unbind();
	}
}