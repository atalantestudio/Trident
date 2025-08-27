#include "pch.hpp"

#include "Engine/Application/Application.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	const Application* Layer::getApplication() const {
		return application;
	}

	void Layer::setApplication(const Application* application) {
		this->application = application;
	}

	void Layer::postConstruct() {}

	void Layer::update(float32 deltaTime, float32 inverseDeltaTime) {}

	void Layer::preRender() {}

	void Layer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {}

	void Layer::postRender() {}

	void Layer::onEvent(Event& event) {}
}