// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Application/Application.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	const Application* Layer::getApplication() const {
		return application;
	}

	void Layer::setApplication(const Application* application) {
		this->application = application;
	}

	void Layer::postConstruct() {}

	void Layer::update(float32 deltaTime, float32 inverseDeltaTime) {}

	void Layer::preRender() {}

	void Layer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {}

	void Layer::postRender() {}

	void Layer::onEvent(Event& event) {}
}