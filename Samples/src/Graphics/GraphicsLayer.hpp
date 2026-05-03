// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Layer/Layer.hpp"

#define DEFINE_SAMPLE_LAYER(name) \
	class name##Layer : public Layer { \
		public: \
			void build() override { \
				renderer.setLayer(this); \
				renderer.build(); \
			} \
			\
			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override { \
				renderer.render(commandEncoder); \
			} \
			\
			void onEvent(Event& event) override { \
				EventDispatcher eventDispatcher(event); \
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize)); \
				\
				renderer.onEvent(event); \
			} \
			\
		private: \
			bool onWindowResize(WindowResizeEvent& event) { \
				renderer.resize(event); \
				\
				return true; \
			} \
			\
		private: \
			name##Renderer renderer; \
	};
