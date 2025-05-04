#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/base.hpp"

namespace ProjectA {
	class Layer {
		public:
			Layer();

			virtual ~Layer() = default;

			const Window& getWindow() const;

			virtual void update(float64 fixedDeltaTime);

			virtual void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder);

			virtual void onEvent(Event& event);
	};
}