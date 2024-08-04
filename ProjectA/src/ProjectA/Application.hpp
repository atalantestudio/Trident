#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "LayerStack.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class API Application {
		private:
			static Application* instance;

			std::unique_ptr<WindowInterface> window;
			ImGuiLayer* imGuiLayer;
			bool running = true;
			LayerStack layerStack;

			std::unique_ptr<Shader> triangleShader;
			std::shared_ptr<VertexArray> triangleVertexArray;

			std::unique_ptr<Shader> squareShader;
			std::shared_ptr<VertexArray> squareVertexArray;

		private:
			bool windowCloseEventCallback(WindowCloseEvent& event);

		public:
			static inline Application& get() {
				return *instance;
			}

			Application();

			virtual ~Application() = default;

			void run();

			void listen(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			inline WindowInterface& getWindow() {
				return *window;
			}
	};

	Application* createApplication();
}