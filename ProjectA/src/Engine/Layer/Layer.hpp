#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"

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