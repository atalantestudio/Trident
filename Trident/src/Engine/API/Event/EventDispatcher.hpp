// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"

namespace Trident {
	class EventDispatcher {
		public:
			explicit EventDispatcher(Event& event) :
				event(event)
			{}

			template<typename T, typename F>
			bool dispatch(const F& function) {
				if (event.getType() != T::getStaticType()) {
					return false;
				}

				event.handled = function(static_cast<T&>(event));

				return true;
			}

		private:
			Event& event;
	};
}