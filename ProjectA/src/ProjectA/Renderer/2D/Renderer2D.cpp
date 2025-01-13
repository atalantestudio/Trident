#include "pch.hpp"

#include "ProjectA/Renderer/2D/Renderer2D.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/Shader.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	struct Renderer2DStorage {
		ref<VertexArray> vertexArray;
		ref<Shader> colorTextureShader;
		ref<Texture2D> defaultTexture;
	};

	static Renderer2DStorage* storage;

	void Renderer2D::construct() {
		PROFILE_SCOPE(__FUNCSIG__);

		storage = new Renderer2DStorage();

		storage->vertexArray = VertexArray::create();

		float32 squareVertices[4 * (3 + 2)] = {
			// Position             Texture coordinates
			-0.5f, -0.5f,  0.0f,    0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,    1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,    1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,    0.0f, 1.0f 
		};

		ref<VertexBuffer> vertexBuffer = VertexBuffer::create(squareVertices, sizeof(squareVertices));

		vertexBuffer->setLayout({
			{
				ShaderType::Float3, "a_position", false
			},
			{
				ShaderType::Float2, "a_uv", false
			}
		});

		storage->vertexArray->addVertexBuffer(vertexBuffer);

		const uint32 squareIndices[6] = {
			0, 1, 2,
			0, 2, 3
		};

		ref<IndexBuffer> squareIndexBuffer = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32));

		storage->vertexArray->setIndexBuffer(squareIndexBuffer);

		storage->colorTextureShader = Shader::create("assets/shaders/ColorTexture.glsl");
		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_texture", 0);

		const uint32 defaultTextureData = 0xffffffff;

		storage->defaultTexture = Texture2D::create(1, 1);
		storage->defaultTexture->setData(&defaultTextureData, 4);
	}

	void Renderer2D::destruct() {
		PROFILE_SCOPE(__FUNCSIG__);

		delete storage;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera) {
		PROFILE_SCOPE(__FUNCSIG__);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_viewProjection", camera.getViewProjection());
	}

	void Renderer2D::endScene() {
		PROFILE_SCOPE(__FUNCSIG__);
	}

	void Renderer2D::drawQuad(const vec2<float>& position, const vec2<float>& size, const vec4<float>& color) {
		drawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const vec3<float>& position, const vec2<float>& size, const vec4<float>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_color", color);
		storage->colorTextureShader->setUniform("u_tilingFactor", 1.0f);
		storage->colorTextureShader->setUniform("u_transform", transform);

		storage->defaultTexture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);
	}

	void Renderer2D::drawQuad(const vec2<float>& position, const vec2<float>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const vec3<float>& position, const vec2<float>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_color", tintColor);
		storage->colorTextureShader->setUniform("u_tilingFactor", tilingFactor);
		storage->colorTextureShader->setUniform("u_transform", transform);

		texture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 rotation, const vec4<float32>& color) {
		drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 rotation, const vec4<float32>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::rotateZ(rotation) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_color", color);
		storage->colorTextureShader->setUniform("u_tilingFactor", 1.0f);
		storage->colorTextureShader->setUniform("u_transform", transform);

		storage->defaultTexture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::rotateZ(rotation) * mat4x4<float>::scale(scale);

		storage->colorTextureShader->bind();
		storage->colorTextureShader->setUniform("u_color", tintColor);
		storage->colorTextureShader->setUniform("u_tilingFactor", tilingFactor);
		storage->colorTextureShader->setUniform("u_transform", transform);

		texture->bind(0);

		storage->vertexArray->bind();

		RenderCommand::drawIndexed(storage->vertexArray);
	}
}