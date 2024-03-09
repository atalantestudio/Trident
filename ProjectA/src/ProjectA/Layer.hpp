#pragma once

#include "core.hpp"
#include "Event/Event.hpp"

namespace ProjectA {
	class API Layer {
		protected:
			std::string debugName;

		public:
			Layer(const std::string& debugName = "Layer");
			virtual ~Layer();

			inline const std::string& getDebugName() const {
				return debugName;
			}

			virtual void attach() {}
			virtual void detach() {}
			virtual void update() {}
			virtual void listen(Event& event) {}
	};
}