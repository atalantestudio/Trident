#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/base.hpp"
#include "Engine/Event/Event.hpp"

namespace ProjectA {
	class Layer {
		public:
			Layer();

			virtual ~Layer() = default;

			virtual void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex);

			virtual void onEvent(Event& event);
	};
}