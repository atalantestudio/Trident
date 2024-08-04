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

		///
		/// Triangle
		///

		triangleVertexArray.reset(VertexArray::create());

		float triangleVertices[3 * 7] = {
		   -0.5f, -0.5f,  0.0f,     0.8f,  0.2f,  0.8f,  1.0f,
			0.5f, -0.5f,  0.0f,     0.2f,  0.3f,  0.8f,  1.0f,
			0.0f,  0.5f,  0.0f,     0.8f,  0.8f,  0.2f,  1.0f
		};

		std::shared_ptr<VertexBuffer> triangleVertexBuffer;
		triangleVertexBuffer.reset(VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));
		triangleVertexBuffer->setLayout({
			{
				ShaderType::Float3, "a_position", false
			},
			{
				ShaderType::Float4, "a_color", false
			}
		});

		triangleVertexArray->addVertexBuffer(triangleVertexBuffer);

		uint32_t triangleIndices[3] = {
			0, 1, 2
		};

		std::shared_ptr<IndexBuffer> triangleIndexBuffer;
		triangleIndexBuffer.reset(IndexBuffer::create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

		triangleVertexArray->setIndexBuffer(triangleIndexBuffer);

		///
		/// Square
		///

		squareVertexArray.reset(VertexArray::create());

		float squareVertices[3 * 4] = {
		   -0.75f, -0.75f,  0.0f,
			0.75f, -0.75f,  0.0f,
			0.75f,  0.75f,  0.0f,
		   -0.75f,  0.75f,  0.0f
		};

		std::shared_ptr<VertexBuffer> squareVertexBuffer;
		squareVertexBuffer.reset(VertexBuffer::create(squareVertices, sizeof(squareVertices)));
		squareVertexBuffer->setLayout({
			{
				ShaderType::Float3, "a_position", false
			}
		});

		squareVertexArray->addVertexBuffer(squareVertexBuffer);

		uint32_t squareIndices[6] = {
			0, 1, 2,
			2, 3, 0
		};

		std::shared_ptr<IndexBuffer> squareIndexBuffer;
		squareIndexBuffer.reset(IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

		squareVertexArray->setIndexBuffer(squareIndexBuffer);

		const std::string& triangleVertexShaderSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec4 a_color;

			out vec3 v_position;
			out vec4 v_color;

			void main() {
				gl_Position = vec4(a_position, 1.0);

				v_position = a_position;
				v_color = a_color;
			}
		)";

		const std::string& triangleFragmentShaderSource = R"(
			#version 330 core

			in vec3 v_position;
			in vec4 v_color;

			layout(location = 0) out vec4 FragColor;

			void main() {
				FragColor = v_color;
			}
		)";

		triangleShader.reset(new Shader(triangleVertexShaderSource, triangleFragmentShaderSource));

		const std::string& squareVertexShaderSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;

			out vec3 v_position;

			void main() {
				gl_Position = vec4(a_position, 1.0);

				v_position = a_position;
			}
		)";

		const std::string& squareFragmentShaderSource = R"(
			#version 330 core

			in vec3 v_position;

			layout(location = 0) out vec4 FragColor;

			void main() {
				FragColor = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		squareShader.reset(new Shader(squareVertexShaderSource, squareFragmentShaderSource));
	}

	bool Application::windowCloseEventCallback(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	void Application::run() {
		while (running) {
			glClearColor(.1f, .1f, .1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			squareShader->bind();
			squareVertexArray->bind();

			glDrawElements(GL_TRIANGLES, squareVertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);

			triangleShader->bind();
			triangleVertexArray->bind();

			glDrawElements(GL_TRIANGLES, triangleVertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);

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
	}

	void Application::pushOverlay(Layer* overlay) {
		layerStack.pushOverlay(overlay);
	}
}