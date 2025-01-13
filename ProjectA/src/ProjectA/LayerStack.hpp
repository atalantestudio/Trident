#pragma once

#include "ProjectA/core.hpp"
#include "ProjectA/Layer.hpp"

namespace ProjectA {
	class LayerStack {
		private:
			std::vector<Layer*> layers;
			unsigned int layerInsertionIndex = 0;

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
	};
}