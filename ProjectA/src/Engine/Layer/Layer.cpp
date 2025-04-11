#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	Layer::Layer() {}

	const Window& Layer::getWindow() const {
		return Application::get().getWindow();
	}

	void Layer::update() {}

	void Layer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {}

	void Layer::onEvent(Event& event) {}
}