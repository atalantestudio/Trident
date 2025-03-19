#pragma once

#include "Engine/base.hpp"
#include "Engine/Layer.hpp"

namespace ProjectA {
	class LayerStack {
		public:
			LayerStack() = default;

			~LayerStack();

			void pushLayer(Layer* layer);
			void pushOverlay(Layer* overlay);
			void removeLayer(Layer* layer);
			void removeOverlay(Layer* overlay);

			// TODO: Write helper macro for iterators.
			std::vector<Layer*>::iterator begin();
			std::vector<Layer*>::iterator end();
			std::vector<Layer*>::reverse_iterator rbegin();
			std::vector<Layer*>::reverse_iterator rend();

		private:
			std::vector<Layer*> layers;
			uint32 layerInsertionIndex = 0;
	};
}