#pragma once

#include "ProjectA/core.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "ProjectA/ImGui/ImGuiLayer.hpp"
#include "ProjectA/LayerStack.hpp"
#include "ProjectA/Window.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		private:
			static Application* instance;

			std::unique_ptr<Window> window;
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

			void onEvent(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			inline Window& getWindow() {
				return *window;
			}

		private:
			void run();

			bool onWindowClose(WindowCloseEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		private:
			friend int ::main(int argc, char** argv);
	};

	Application* createApplication();
}