#include "pch.hpp"

#include "ProjectA/Renderer/2D/Renderer2D.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/Shader.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	struct Renderer2DStorage {
		Ref<VertexArray> vertexArray;
		Ref<Shader> colorTextureShader;
		Ref<Texture2D> defaultTexture;
	};

	static Renderer2DStorage* storage;

	void Renderer2D::construct() {
		PROFILE_SCOPE(__FUNCSIG__);

		storage = new Renderer2DStorage();

		storage->vertexArray = VertexArray::create();

		std::float_t squareVertices[4 * (3 + 2)] = {
			// Position             Texture coordinates
			-0.5f, -0.5f,  0.0f,    0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,    1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,    1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,    0.0f, 1.0f
		};

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(squareVertices, sizeof(squareVertices));

		vertexBuffer->setLayout({
			{
				ShaderType::Float3, "a_position", false
			},
			{
				ShaderType::Float2, "a_uv", false
			}
		});

		storage->vertexArray->addVertexBuffer(vertexBuffer);

		std::uint32_t squareIndices[6] = {
			0, 1, 2,
			0, 2, 3
		};

		Ref<IndexBuffer> squareIndexBuffer = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(std::uint32_t));

		storage->vertexArray->setIndexBuffer(squareIndexBuffer);

		storage->colorTextureShader = Shader::create("assets/shaders/ColorTexture.glsl");

		storage->defaultTexture = Texture2D::create(1, 1);

		const std::uint32_t defaultTextureData = 0xffffffff;

		storage->defaultTexture->setData(&defaultTextureData);
	}

	void Renderer2D::destruct() {
		PROFILE_SCOPE(__FUNCSIG__);

		delete storage;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera) {
		PROFILE_SCOPE(__FUNCSIG__);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_viewProjection", camera.getViewProjection());
		storage->colorTextureShader->setUniform("u_texture", 0);
	}

	void Renderer2D::endScene() {
		PROFILE_SCOPE(__FUNCSIG__);
	}

	void Renderer2D::drawQuad(const vec2<float>& position, const vec2<float>& size, const vec4<float>& color) {
		drawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const vec3<float>& position, const vec2<float>& size, const vec4<float>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		vec3<float> scale = vec3<float>(size.x, size.y, 1.0f);
		mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->setUniform("u_transform", transform);
		storage->colorTextureShader->setUniform("u_color", color);

		storage->defaultTexture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);

		storage->defaultTexture->unbind();
	}

	void Renderer2D::drawTexturedQuad(const vec2<float>& position, const vec2<float>& size, const Ref<Texture2D>& texture, float textureSize = 1.0f) {
		drawTexturedQuad({position.x, position.y, 0.0f}, size, texture, textureSize);
	}

	void Renderer2D::drawTexturedQuad(const vec3<float>& position, const vec2<float>& size, const Ref<Texture2D>& texture, float textureSize = 1.0f) {
		PROFILE_SCOPE(__FUNCSIG__);

		vec3<float> scale = vec3<float>(size.x, size.y, 1.0f);
		mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->setUniform("u_transform", transform);
		storage->colorTextureShader->setUniform("u_color", {1.0f, 1.0f, 1.0f, 1.0f});
		storage->colorTextureShader->setUniform("u_texture_size", textureSize);

		texture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);

		texture->unbind();
	}
}