#include "pch.hpp"
#include "Application.hpp"
#include <glad/glad.h>

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application::Application() {
		CORE_ASSERT(!instance, "The Application instance already exists.")

		instance = this;

		window = std::unique_ptr<WindowInterface>(WindowInterface::create());
		window->setEventCallback(BIND_FUNCTION(Application::listen));

		imGuiLayer = new ImGuiLayer();

		pushOverlay(imGuiLayer);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		float vertices[9] = {
			-.5f, -.5f, 0,
			.5f, -.5f, 0,
			0, .5f, 0
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		unsigned short indices[3] = {0, 1, 2};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		const std::string& vertexShaderSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;

			out vec3 v_position;

			void main() {
				gl_Position = vec4(a_position, 1.);

				v_position = a_position;
			}
		)";

		const std::string& fragmentShaderSource = R"(
			#version 330 core

			in vec3 v_position;

			layout(location = 0) out vec4 FragColor;

			void main() {
				FragColor = vec4(v_position * .5 + .5, 1.);
				// FragColor = vec4(.8, .2, .2, 1.);
			}
		)";

		shader.reset(new Shader(vertexShaderSource, fragmentShaderSource));
	}

	Application::~Application() {}

	bool Application::windowCloseEventCallback(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	void Application::run() {
		while (running) {
			glClearColor(.1f, .1f, .1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			shader->bind();

			glBindVertexArray(vertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

			for (Layer* layer : layerStack) {
				layer->update();
			}

			imGuiLayer->begin();

			for (Layer* layer : layerStack) {
				layer->renderImGui();
			}

			imGuiLayer->end();

			window->update();
		}
	}

	void Application::listen(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<WindowCloseEvent>(BIND_FUNCTION(Application::windowCloseEventCallback));

		for (auto i = layerStack.end(); i != layerStack.begin();) {
			(*--i)->listen(event);

			if (event.handled) {
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer) {
		layerStack.pushLayer(layer);

		layer->attach();
	}

	void Application::pushOverlay(Layer* overlay) {
		layerStack.pushOverlay(overlay);

		overlay->attach();
	}
}