#include "pch.hpp"

#include "Engine/Layer/LayerStack.hpp"

namespace ProjectA {
	LayerStack::~LayerStack() {
		// TODO: Make implicit?
		for (Layer* layer : layers) {
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer) {
		layers.emplace(layers.begin() + layerInsertionIndex, layer);

		layerInsertionIndex += 1;
	}

	void LayerStack::pushOverlay(Layer* overlay) {
		layers.emplace_back(overlay);
	}

	void LayerStack::removeLayer(Layer* layer) {
		const chain<Layer*>::const_iterator iterator = std::find(layers.begin(), layers.begin() + layerInsertionIndex, layer);

		if (iterator == layers.begin() + layerInsertionIndex) {
			return;
		}

		layers.erase(iterator);

		layerInsertionIndex -= 1;
	}

	void LayerStack::removeOverlay(Layer* overlay) {
		const chain<Layer*>::const_iterator iterator = std::find(layers.begin() + layerInsertionIndex, layers.end(), overlay);

		if (iterator == layers.end()) {
			return;
		}

		layers.erase(iterator);
	}

	chain<Layer*>::iterator LayerStack::begin() {
		return layers.begin();
	}

	chain<Layer*>::iterator LayerStack::end() {
		return layers.end();
	}

	chain<Layer*>::reverse_iterator LayerStack::rbegin() {
		return layers.rbegin();
	}

	chain<Layer*>::reverse_iterator LayerStack::rend() {
		return layers.rend();
	}
}