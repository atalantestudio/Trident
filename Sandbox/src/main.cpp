#include "ProjectA.hpp"

class TestLayer : public ProjectA::Layer {
	private:
		std::shared_ptr<ProjectA::Shader> triangleShader;
		std::shared_ptr<ProjectA::VertexArray> triangleVertexArray;

		std::shared_ptr<ProjectA::Shader> squareShader;
		std::shared_ptr<ProjectA::VertexArray> squareVertexArray;

		ProjectA::OrthographicCamera camera;
		ProjectA::vec3<float> cameraPosition;
		float cameraRotation;
		float cameraMoveVelocity;
		float cameraRotationVelocity;

	public:
		TestLayer() :
			Layer("Test"),
			camera(-1.6f, 1.6f, -0.9f, 0.9f, -1.0f, 1.0f),
			cameraPosition(0.0f, 0.0f, 0.0f),
			cameraRotation(0.0f),
			cameraMoveVelocity(4.0f),
			cameraRotationVelocity(180.0f)
		{
			///
			/// Triangle
			///

			triangleVertexArray.reset(ProjectA::VertexArray::create());

			float triangleVertices[3 * 7] = {
			   -0.5f, -0.5f,  0.0f,     0.8f,  0.2f,  0.8f,  1.0f,
				0.5f, -0.5f,  0.0f,     0.2f,  0.3f,  0.8f,  1.0f,
				0.0f,  0.5f,  0.0f,     0.8f,  0.8f,  0.2f,  1.0f
			};

			std::shared_ptr<ProjectA::VertexBuffer> triangleVertexBuffer;
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

			std::shared_ptr<ProjectA::IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer.reset(ProjectA::IndexBuffer::create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

			triangleVertexArray->setIndexBuffer(triangleIndexBuffer);

			///
			/// Square
			///

			squareVertexArray.reset(ProjectA::VertexArray::create());

			float squareVertices[3 * 4] = {
			   -0.5f, -0.5f,  0.0f,
				0.5f, -0.5f,  0.0f,
				0.5f,  0.5f,  0.0f,
			   -0.5f,  0.5f,  0.0f
			};

			std::shared_ptr<ProjectA::VertexBuffer> squareVertexBuffer;
			squareVertexBuffer.reset(ProjectA::VertexBuffer::create(squareVertices, sizeof(squareVertices)));
			squareVertexBuffer->setLayout({
				{
					ProjectA::ShaderType::Float3, "a_position", false
				}
			});

			squareVertexArray->addVertexBuffer(squareVertexBuffer);

			uint32_t squareIndices[6] = {
				0, 1, 2,
				2, 3, 0
			};

			std::shared_ptr<ProjectA::IndexBuffer> squareIndexBuffer;
			squareIndexBuffer.reset(ProjectA::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

			squareVertexArray->setIndexBuffer(squareIndexBuffer);

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

			triangleShader.reset(new ProjectA::Shader(triangleVertexShaderSource, triangleFragmentShaderSource));

			const std::string& squareVertexShaderSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_position;

				uniform mat4 u_viewProjection;
				uniform mat4 u_transform; // TODO: Change to attribute

				out vec3 v_position;

				void main() {
					gl_Position = vec4(a_position, 1.0f) * u_transform * u_viewProjection;

					v_position = a_position;
				}
			)";

			const std::string& squareFragmentShaderSource = R"(
				#version 330 core

				in vec3 v_position;

				layout(location = 0) out vec4 FragColor;

				void main() {
					FragColor = vec4(0.2f, 0.3f, 0.8f, 1.0f);
				}
			)";

			squareShader.reset(new ProjectA::Shader(squareVertexShaderSource, squareFragmentShaderSource));
		}

		void update(float deltaTime) override {
			if (ProjectA::InputInterface::isKeyPressed(KEY_LEFT)) {
				cameraPosition[0] -= cameraMoveVelocity * deltaTime;
			} else if (ProjectA::InputInterface::isKeyPressed(KEY_RIGHT)) {
				cameraPosition[0] += cameraMoveVelocity * deltaTime;
			}

			if (ProjectA::InputInterface::isKeyPressed(KEY_UP)) {
				cameraPosition[1] += cameraMoveVelocity * deltaTime;
			} else if (ProjectA::InputInterface::isKeyPressed(KEY_DOWN)) {
				cameraPosition[1] -= cameraMoveVelocity * deltaTime;
			}

			if (ProjectA::InputInterface::isKeyPressed(KEY_Q)) {
				cameraRotation += cameraRotationVelocity * deltaTime;
			} else if (ProjectA::InputInterface::isKeyPressed(KEY_E)) {
				cameraRotation -= cameraRotationVelocity * deltaTime;
			}

			ProjectA::RenderCommand::setClearColor(ProjectA::vec4<float>(0.1f, 0.1f, 0.1f, 1.0f));
			ProjectA::RenderCommand::clear();

			camera.setPosition(cameraPosition);
			camera.setRotation(cameraRotation);

			static const ProjectA::mat4x4<float> scale(
				0.1, 0, 0, 0,
				0, 0.1, 0, 0,
				0, 0, 0.1, 0,
				0, 0, 0, 1
			);

			ProjectA::Renderer::beginScene(camera);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					ProjectA::vec3<float> position(x * 0.11, y * 0.11, 0);
					ProjectA::mat4x4<float> transform = ProjectA::mat4x4<float>::translate(position) * scale;

					ProjectA::Renderer::submit(squareShader, squareVertexArray, transform);
				}
			}

			ProjectA::Renderer::submit(triangleShader, triangleVertexArray, ProjectA::mat4x4<float>(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			));

			ProjectA::Renderer::endScene();
		}

		virtual void renderImGui() override {}

		void listen(ProjectA::Event& event) override {}
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