#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/Layer/LayerInput.hpp"

namespace ProjectA {
	class Application;

	// TODO: Rename to Subpass?
	class Layer {
		public:
			virtual ~Layer() = default;

			const Application* getApplication() const;

			void setApplication(const Application* application);

			virtual void build() = 0;

			virtual void postConstruct();

			virtual void update(float32 deltaTime, float32 inverseDeltaTime);

			virtual void preRender();

			virtual void render(float64 remainingFrameTime, ref<CommandEncoder> commandEncoder) const;

			virtual void postRender();

			virtual void onEvent(Event& event);

		public:
			LayerInput* input;

		private:
			const Application* application;
	};
}