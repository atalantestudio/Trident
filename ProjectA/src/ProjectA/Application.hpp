#pragma once

#include "ProjectA/base.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "ProjectA/ImGui/ImGuiLayer.hpp"
#include "ProjectA/LayerStack.hpp"
#include "ProjectA/Window.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		public:
			ImGuiLayer* imGuiLayer;

		private:
			static Application* instance;

		private:
			std::unique_ptr<Window> window;
			LayerStack layerStack;
			bool running = true;
			bool minimized = false;
			float32 lastFrameTime;

		public:
			static Application& get() {
				return *instance;
			}

			Application(const std::string& name = "Application");

			virtual ~Application();

			Window& getWindow();

			void onEvent(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			void close();

		private:
			void run();

			bool onWindowClose(WindowCloseEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		private:
			friend int ::main(int argc, char** argv);
	};

	Application* createApplication();
}