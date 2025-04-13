#include "pch.hpp"

#include "Engine/Layer/LayerStack.hpp"

namespace ProjectA {
	void LayerStack::pushLayer(const ref<Layer>& layer) {
		layers.emplace(layers.begin() + layerInsertionIndex, layer);

		layerInsertionIndex += 1;
	}

	void LayerStack::pushOverlay(const ref<Layer>& overlay) {
		layers.emplace_back(overlay);
	}

	void LayerStack::removeLayer(const ref<Layer>& layer) {
		const chain<ref<Layer>>::const_iterator iterator = std::find(layers.begin(), layers.begin() + layerInsertionIndex, layer);

		if (iterator == layers.begin() + layerInsertionIndex) {
			return;
		}

		layers.erase(iterator);

		layerInsertionIndex -= 1;
	}

	void LayerStack::removeOverlay(const ref<Layer>& overlay) {
		const chain<ref<Layer>>::const_iterator iterator = std::find(layers.begin() + layerInsertionIndex, layers.end(), overlay);

		if (iterator == layers.end()) {
			return;
		}

		layers.erase(iterator);
	}

	chain<ref<Layer>>::iterator LayerStack::begin() {
		return layers.begin();
	}

	chain<ref<Layer>>::iterator LayerStack::end() {
		return layers.end();
	}

	chain<ref<Layer>>::reverse_iterator LayerStack::rbegin() {
		return layers.rbegin();
	}

	chain<ref<Layer>>::reverse_iterator LayerStack::rend() {
		return layers.rend();
	}
}