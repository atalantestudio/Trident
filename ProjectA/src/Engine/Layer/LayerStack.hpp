#pragma once

#include "Engine/base.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class LayerStack {
		public:
			void pushLayer(const ref<Layer>& layer);
			void pushOverlay(const ref<Layer>& overlay);
			void removeLayer(const ref<Layer>& layer);
			void removeOverlay(const ref<Layer>& overlay);

			const ref<Layer>& operator [](std::size_t index) const;

			// TODO: Write helper macro for iterators.
			chain<ref<Layer>>::iterator begin();

			chain<ref<Layer>>::iterator end();

			chain<ref<Layer>>::reverse_iterator rbegin();

			chain<ref<Layer>>::reverse_iterator rend();

		private:
			chain<ref<Layer>> layers;
			uint32 layerInsertionIndex = 0;
	};
}