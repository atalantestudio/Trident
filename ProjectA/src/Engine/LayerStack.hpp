#pragma once

#include "Engine/base.hpp"
#include "Engine/Layer.hpp"

namespace ProjectA {
	class LayerStack {
		private:
			std::vector<Layer*> layers;
			uint32 layerInsertionIndex = 0;

		public:
			LayerStack() = default;

			~LayerStack();

			void pushLayer(Layer* layer);
			void pushOverlay(Layer* overlay);
			void removeLayer(Layer* layer);
			void removeOverlay(Layer* overlay);

			std::vector<Layer*>::iterator begin() {
				return layers.begin();
			}

			std::vector<Layer*>::iterator end() {
				return layers.end();
			}

			std::vector<Layer*>::reverse_iterator rbegin() {
				return layers.rbegin();
			}

			std::vector<Layer*>::reverse_iterator rend() {
				return layers.rend();
			}
	};
}