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
			std::vector<ref<Layer>>::iterator begin();

			std::vector<ref<Layer>>::iterator end();

			std::vector<ref<Layer>>::reverse_iterator rbegin();

			std::vector<ref<Layer>>::reverse_iterator rend();

		private:
			std::vector<ref<Layer>> layers;
			uint32 layerInsertionIndex = 0;
	};
}