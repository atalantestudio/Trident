#include "pch.hpp"

#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	Layer::Layer() {}

	void Layer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {}

	void Layer::onEvent(Event& event) {}
}