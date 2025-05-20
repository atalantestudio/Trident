#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class Layer {
		public:
			Layer() {}

			virtual ~Layer() = default;

			virtual void update(float64 fixedDeltaTime) {}

			virtual void preRender() {}

			virtual void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {}

			virtual void postRender() {}

			virtual void onEvent(Event& event) {}
	};
}