#include "imgui/imgui.h"
#include "ProjectA.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

class TestLayer : public ProjectA::Layer {
	private:
		ProjectA::ShaderLibrary shaderLibrary;
		ProjectA::Ref<ProjectA::Shader> triangleShader;
		ProjectA::Ref<ProjectA::VertexArray> triangleVertexArray;

		ProjectA::Ref<ProjectA::Shader> squareShader;
		ProjectA::Ref<ProjectA::VertexArray> squareVertexArray;

		ProjectA::Ref<ProjectA::Texture2D> plasterWallTexture;
		ProjectA::Ref<ProjectA::Texture2D> decalGraffitiTexture;

		ProjectA::OrthographicCameraController cameraController;

		ProjectA::vec3<float> squareColor;

	public:
		TestLayer() : Layer("Test"), cameraController(1280.0f / 720.0f), squareColor(0.2f, 0.3f, 0.8f) {
			///
			/// Triangle
			///

			triangleVertexArray.reset(ProjectA::VertexArray::create());

			float triangleVertices[3 * 7] = {
			   -0.5f, -0.5f,  0.0f,     0.8f,  0.2f,  0.8f,  1.0f,
				0.5f, -0.5f,  0.0f,     0.2f,  0.3f,  0.8f,  1.0f,
				0.0f,  0.5f,  0.0f,     0.8f,  0.8f,  0.2f,  1.0f
			};

			ProjectA::Ref<ProjectA::VertexBuffer> triangleVertexBuffer;
			triangleVertexBuffer.reset(ProjectA::VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));
			triangleVertexBuffer->setLayout({
				{
					ProjectA::ShaderType::Float3, "a_position", false
				},
				{
					ProjectA::ShaderType::Float4, "a_color", false
				}
			});

			triangleVertexArray->addVertexBuffer(triangleVertexBuffer);

			uint32_t triangleIndices[3] = {
				0, 1, 2
			};

			ProjectA::Ref<ProjectA::IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer.reset(ProjectA::IndexBuffer::create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

			triangleVertexArray->setIndexBuffer(triangleIndexBuffer);

			///
			/// Square
			///

			squareVertexArray.reset(ProjectA::VertexArray::create());

			float squareVertices[5 * 4] = {
			   -0.5f, -0.5f,  0.0f,     0.0f, 0.0f,
				0.5f, -0.5f,  0.0f,     1.0f, 0.0f,
				0.5f,  0.5f,  0.0f,     1.0f, 1.0f,
			   -0.5f,  0.5f,  0.0f,     0.0f, 1.0f
			};

			ProjectA::Ref<ProjectA::VertexBuffer> squareVertexBuffer;
			squareVertexBuffer.reset(ProjectA::VertexBuffer::create(squareVertices, sizeof(squareVertices)));
			squareVertexBuffer->setLayout({
				{
					ProjectA::ShaderType::Float3, "a_position", false
				},
				{
					ProjectA::ShaderType::Float2, "a_uv", false
				}
			});

			// TODO: Design BufferLayout
			/* vertexBuffer.setLayout({
				Attribute<vec3<f32>>("a_position"),
				Attribute<vec2<f32>>("a_uv"),
			}); */

			squareVertexArray->addVertexBuffer(squareVertexBuffer);

			uint32_t squareIndices[6] = {
				0, 1, 2,
				0, 2, 3
			};

			ProjectA::Ref<ProjectA::IndexBuffer> squareIndexBuffer;
			squareIndexBuffer.reset(ProjectA::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

			squareVertexArray->setIndexBuffer(squareIndexBuffer);

			/// 
			/// Triangle shader
			/// 

			const std::string& triangleVertexShaderSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_position;
				layout(location = 1) in vec4 a_color;

				uniform mat4 u_viewProjection;
				uniform mat4 u_transform; // TODO: Change to attribute

				out vec3 v_position;
				out vec4 v_color;

				void main() {
					gl_Position = vec4(a_position, 1.0f) * u_transform * u_viewProjection;

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

			triangleShader = ProjectA::Shader::create(triangleVertexShaderSource, triangleFragmentShaderSource, "triangle");

			/// 
			/// Square shader
			/// 

			const std::string& squareVertexShaderSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_position;

				uniform mat4 u_viewProjection;
				uniform mat4 u_transform;

				void main() {
					gl_Position = vec4(a_position, 1.0f) * u_transform * u_viewProjection;
				}
			)";

			const std::string& squareFragmentShaderSource = R"(
				#version 330 core

				uniform vec3 u_color;

				layout(location = 0) out vec4 FragColor;

				void main() {
					FragColor = vec4(u_color, 1.0f);
				}
			)";

			squareShader = ProjectA::Shader::create(squareVertexShaderSource, squareFragmentShaderSource, "square");

			auto textureShader = shaderLibrary.load("assets/shaders/texture.glsl");

			plasterWallTexture = ProjectA::Texture2D::create("assets/textures/plasterwall030c.jpg");
			decalGraffitiTexture = ProjectA::Texture2D::create("assets/textures/decalgraffiti001d.png");

			textureShader->bind();
			std::dynamic_pointer_cast<ProjectA::OpenGLShader>(textureShader)->uploadUniform("u_texture", 0);
		}

		void update(float deltaTime) override {
			cameraController.update(deltaTime);

			ProjectA::RenderCommand::setClearColor(ProjectA::vec4<float>(0.1f, 0.1f, 0.1f, 1.0f));
			ProjectA::RenderCommand::clear();

			static const ProjectA::mat4x4<float> squareScale(
				0.1, 0, 0, 0,
				0, 0.1, 0, 0,
				0, 0, 0.1, 0,
				0, 0, 0, 1
			);

			ProjectA::Renderer::beginScene(cameraController.getCamera());

			squareShader->bind();
			std::dynamic_pointer_cast<ProjectA::OpenGLShader>(squareShader)->uploadUniform("u_color", squareColor);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					ProjectA::vec3<float> position(x * 0.11, y * 0.11, 0);
					ProjectA::mat4x4<float> transform = ProjectA::mat4x4<float>::translate(position) * squareScale;

					ProjectA::Renderer::submit(squareShader, squareVertexArray, transform);
				}
			}

			static const ProjectA::mat4x4<float> textureScale(
				1.5, 0, 0, 0,
				0, 1.5, 0, 0,
				0, 0, 1.5, 0,
				0, 0, 0, 1
			);

			auto textureShader = shaderLibrary.get("texture");

			plasterWallTexture->bind(0);
			ProjectA::Renderer::submit(textureShader, squareVertexArray, textureScale);

			decalGraffitiTexture->bind(0);
			ProjectA::Renderer::submit(textureShader, squareVertexArray, textureScale);

			ProjectA::Renderer::endScene();
		}

		virtual void renderImGui() override {
			ImGui::Begin("Settings");
				ImGui::ColorPicker3("Square color", squareColor.entries);
			ImGui::End();
		}

		void onEvent(ProjectA::Event& event) override {
			cameraController.onEvent(event);
		}
};

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication() {
			pushLayer(new TestLayer());
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}