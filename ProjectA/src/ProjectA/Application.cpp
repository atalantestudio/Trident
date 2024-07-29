#include "pch.hpp"
#include "Application.hpp"
#include <glad/glad.h>

namespace ProjectA {
	// For debug purposes
	static GLenum getShaderType(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
				return GL_BOOL;
			case ShaderType::Int:
				return GL_INT;
			case ShaderType::Int2:
				return GL_INT;
			case ShaderType::Int3:
				return GL_INT;
			case ShaderType::Int4:
				return GL_INT;
			case ShaderType::Float:
				return GL_FLOAT;
			case ShaderType::Float2:
				return GL_FLOAT;
			case ShaderType::Float3:
				return GL_FLOAT;
			case ShaderType::Float4:
				return GL_FLOAT;
			case ShaderType::Mat3:
				return GL_FLOAT;
			case ShaderType::Mat4:
				return GL_FLOAT;
		}

		CORE_ASSERT(false, "Unknown ShaderType");

		return 0;
	}

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

		// [position, color]
		float vertices[3 * 7] = {
		   -0.5f, -0.5f, 0,     0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0,     0.2f, 0.3f, 0.8f, 1.0f,
			0,     0.5f, 0,     0.8f, 0.8f, 0.2f, 1.0f
		};

		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		{
			BufferLayout layout({
				{
					ShaderType::Float3,
					"a_position",
					false
				},
				{
					ShaderType::Float4,
					"a_color",
					false
				}
			});

			vertexBuffer->setLayout(layout);
		}

		const auto& layout = vertexBuffer->getLayout();

		uint32_t attributeIndex = 0;

		for (const auto& element : layout) {
			glEnableVertexAttribArray(attributeIndex);
			glVertexAttribPointer(
				attributeIndex,
				getShaderTypeComponentCount(element.type),
				getShaderType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*) element.offset
			);

			attributeIndex++;
		}

		uint32_t indices[3] = {0, 1, 2};

		indexBuffer.reset(IndexBuffer::create(indices, 3));

		const std::string& vertexShaderSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec4 a_color;

			out vec3 v_position;
			out vec4 v_color;

			void main() {
				gl_Position = vec4(a_position, 1.);

				v_position = a_position;
				v_color = a_color;
			}
		)";

		const std::string& fragmentShaderSource = R"(
			#version 330 core

			in vec3 v_position;
			in vec4 v_color;

			layout(location = 0) out vec4 FragColor;

			void main() {
				FragColor = v_color;
			}
		)";

		shader.reset(new Shader(vertexShaderSource, fragmentShaderSource));
	}

	bool Application::windowCloseEventCallback(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	void Application::run() {
		while (running) {
			glClearColor(.1f, .1f, .1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			indexBuffer->bind();
			vertexBuffer->bind();
			shader->bind();

			glBindVertexArray(vertexArray);
			glDrawElements(GL_TRIANGLES, indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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