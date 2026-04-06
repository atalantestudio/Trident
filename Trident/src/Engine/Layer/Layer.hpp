// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/Layer/LayerInput.hpp"

namespace Trident {
	class Application;

	// TODO: Rename to Subpass?
	class Layer {
		public:
			virtual ~Layer() = default;

			const Application* getApplication() const;
			Application* getApplication();

			void setApplication(Application* application);

			virtual void build() = 0;

			virtual void postConstruct();

			virtual void update(float32 deltaTime, float32 inverseDeltaTime);

			virtual void preRender();

			virtual void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const;

			virtual void postRender();

			virtual void onEvent(Event& event);

		public:
			LayerInput* input;

		private:
			Application* application;
	};
}