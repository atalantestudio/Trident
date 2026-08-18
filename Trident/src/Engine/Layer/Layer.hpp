// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Descriptor/StaticMemoryDescriptor.hpp"
#include "Engine/Layer/LayerInput.hpp"

namespace Trident {
	class Application;

	class Layer {
		public:
			virtual ~Layer() = default;

			const Application* getApplication() const;
			Application* getApplication();

			void setApplication(Application* application);

			virtual void initialize() = 0;

			/// Returns the layer's requirements regarding static memory allocations.
			// TODO(layer/initialize): Make pure virtual.
			// virtual StaticMemoryDescriptor getStaticMemoryDescriptor() const = 0;
			virtual StaticMemoryDescriptor getStaticMemoryDescriptor() const {
				return {};
			}

			// TODO(layer/initialize): Make pure virtual.
			// virtual void encodeOneTimeCommands() = 0;
			virtual void encodeOneTimeCommands() {}

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