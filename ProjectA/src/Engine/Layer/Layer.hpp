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

			virtual void update();

			virtual void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex);

			virtual void onEvent(Event& event);
	};
}