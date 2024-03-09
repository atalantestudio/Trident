#pragma once

#include "core.hpp"
#include "Layer.hpp"
#include <vector>

namespace ProjectA {
	class API LayerStack {
		private:
			std::vector<Layer*> layers;
			std::vector<Layer*>::iterator layerInsertionIterator;

		public:
			LayerStack();
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
	};
}