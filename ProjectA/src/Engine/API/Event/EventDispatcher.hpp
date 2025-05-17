#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	// TODO: Only use EventDispatcher in Application (not in layers).
	class EventDispatcher {
		public:
			EventDispatcher(Event& event) :
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