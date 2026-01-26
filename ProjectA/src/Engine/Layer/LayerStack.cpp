#include "pch.hpp"

#include "Engine/Layer/LayerStack.hpp"

namespace ProjectA {
	uint32 LayerStack::count() const {
		return static_cast<uint32>(layers.size());
	}

	void LayerStack::pushLayer(ref<Layer> layer) {
		layers.emplace(layers.begin() + layerInsertionIndex, layer);

		layerInsertionIndex += 1;
	}

	void LayerStack::removeLayer(ref<Layer> layer) {
		const std::vector<ref<Layer>>::const_iterator iterator = std::find(layers.begin(), layers.begin() + layerInsertionIndex, layer);

		if (iterator == layers.begin() + layerInsertionIndex) {
			return;
		}

		layers.erase(iterator);

		layerInsertionIndex -= 1;
	}

	bool LayerStack::hasLayers() const {
		return layerInsertionIndex > 0;
	}

	void LayerStack::clear() {
		layers.clear();
	}

	ref<Layer> LayerStack::operator [](std::size_t index) const {
		ENGINE_ASSERT(index < layers.size(), "Index overflow.");

		return layers[index];
	}

	std::vector<ref<Layer>>::iterator LayerStack::begin() {
		return layers.begin();
	}

	std::vector<ref<Layer>>::iterator LayerStack::end() {
		return layers.end();
	}

	std::vector<ref<Layer>>::reverse_iterator LayerStack::rbegin() {
		return layers.rbegin();
	}

	std::vector<ref<Layer>>::reverse_iterator LayerStack::rend() {
		return layers.rend();
	}
}