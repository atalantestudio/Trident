#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	// TODO: Rename to Subpass.
	class Layer {
		public:
			virtual ~Layer() = default;

			virtual void build() = 0;

			virtual void postConstruct() {}

			virtual void update(float64 fixedDeltaTime) {}

			virtual void preRender() {}

			virtual void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {}

			virtual void postRender() {}

			virtual void onEvent(Event& event) {}
	};
}