#pragma once

#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class LayerStack {
		public:
			void pushLayer(ref<Layer> layer);

			void removeLayer(ref<Layer> layer);

			bool hasLayers() const;

			ref<Layer> operator [](std::size_t index) const;

			std::vector<ref<Layer>>::iterator begin();

			std::vector<ref<Layer>>::iterator end();

			std::vector<ref<Layer>>::reverse_iterator rbegin();

			std::vector<ref<Layer>>::reverse_iterator rend();

		private:
			std::vector<ref<Layer>> layers;
			uint32 layerInsertionIndex = 0;
	};
}