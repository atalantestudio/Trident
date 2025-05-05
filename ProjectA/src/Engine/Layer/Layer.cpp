#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	Layer::Layer() {}

	const Window& Layer::getWindow() const {
		return Application::get().getWindow();
	}

	void Layer::update(float64 fixedDeltaTime) {}

	void Layer::preRender() {}

	void Layer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {}

	void Layer::postRender() {}

	void Layer::onEvent(Event& event) {}
}