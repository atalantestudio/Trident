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
			chain<Layer*>::iterator begin();
			chain<Layer*>::iterator end();
			chain<Layer*>::reverse_iterator rbegin();
			chain<Layer*>::reverse_iterator rend();

		private:
			chain<Layer*> layers;
			uint32 layerInsertionIndex = 0;
	};
}