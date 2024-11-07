#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "LayerStack.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class Application {
		private:
			static Application* instance;

			std::unique_ptr<WindowInterface> window;
			ImGuiLayer* imGuiLayer;
			LayerStack layerStack;

			bool running = true;
			bool minimized = false;

			float lastFrameTime;

		public:
			static inline Application& get() {
				return *instance;
			}

			Application();

			virtual ~Application();

			void run();

			void onEvent(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			inline WindowInterface& getWindow() {
				return *window;
			}

		private:
			bool onWindowClose(WindowCloseEvent& event);
			bool onWindowResize(WindowResizeEvent& event);
	};

	Application* createApplication();
}