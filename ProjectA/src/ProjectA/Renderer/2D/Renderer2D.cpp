#include "pch.hpp"

#include "ProjectA/Renderer/2D/Renderer2D.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/Shader.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	struct QuadVertex {
		vec3<float32> position;
		vec4<float32> color;
		vec2<float32> uv;
		int32 textureSlot;
		float32 tilingFactor;
	};

	struct Renderer2DStorage {
		static constexpr uint32 MAX_QUADS = 10000;
		static constexpr uint32 MAX_INDICES = MAX_QUADS * 6;
		static constexpr uint32 MAX_VERTICES = MAX_QUADS * 4;

		// TODO: Capabilities class
		static constexpr uint32 MAX_TEXTURE_SLOTS = 32;

		ref<Shader> colorTextureShader;
		ref<VertexArray> vertexArray;
		ref<VertexBuffer> vertexBuffer;
		uint32 quadCount = 0;
		QuadVertex* vertexBufferOrigin = nullptr;
		QuadVertex* vertexBufferPointer = nullptr;

		// TODO: Replace with assets/asset containers
		std::array<ref<Texture2D>, MAX_TEXTURE_SLOTS> textureSlots;
		uint32 currentSlotIndex = 1; // Skip default texture
	};

	static Renderer2DStorage storage;

	void Renderer2D::construct() {
		PROFILE_SCOPE(__FUNCSIG__);

		storage.colorTextureShader = Shader::create("assets/shaders/ColorTexture.glsl");
		storage.colorTextureShader->bind();

		storage.vertexArray = VertexArray::create();

		storage.vertexBuffer = VertexBuffer::create(sizeof(QuadVertex) * Renderer2DStorage::MAX_VERTICES);
		storage.vertexBuffer->setLayout({
			{
				ShaderType::Float3, "a_position", false
			},
			{
				ShaderType::Float4, "a_color", false
			},
			{
				ShaderType::Float2, "a_uv", false
			},
			{
				ShaderType::Int, "a_texture", false
			},
			{
				ShaderType::Float, "a_tilingFactor", false
			}
		});

		storage.vertexArray->addVertexBuffer(storage.vertexBuffer);

		storage.vertexBufferOrigin = new QuadVertex[Renderer2DStorage::MAX_VERTICES];

		uint32* indices = new uint32[Renderer2DStorage::MAX_INDICES];

		for (uint32 index = 0, indexOffset = 0; index < Renderer2DStorage::MAX_INDICES - 5; index += 6) {
			indices[index + 0] = indexOffset + 0;
			indices[index + 1] = indexOffset + 1;
			indices[index + 2] = indexOffset + 2;

			indices[index + 3] = indexOffset + 0;
			indices[index + 4] = indexOffset + 2;
			indices[index + 5] = indexOffset + 3;

			indexOffset += 4;
		}

		ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, Renderer2DStorage::MAX_INDICES);

		delete[] indices;

		storage.vertexArray->setIndexBuffer(indexBuffer);

		const uint32 defaultTextureData = 0xffffffff;

		storage.textureSlots[0] = Texture2D::create(1, 1);
		storage.textureSlots[0]->setData(&defaultTextureData, 4);

		int32 textureSamplers[Renderer2DStorage::MAX_TEXTURE_SLOTS];

		for (int32 i = 0; i < Renderer2DStorage::MAX_TEXTURE_SLOTS; i++) {
			textureSamplers[i] = i;
		}

		storage.colorTextureShader->setUniform("u_textures", textureSamplers, Renderer2DStorage::MAX_TEXTURE_SLOTS);
	}

	void Renderer2D::destruct() {
		delete[] storage.vertexBufferOrigin;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera) {
		PROFILE_SCOPE(__FUNCSIG__);

		storage.colorTextureShader->bind();
		storage.colorTextureShader->setUniform("u_viewProjection", camera.getViewProjection());

		storage.quadCount = 0;
		storage.vertexBufferPointer = storage.vertexBufferOrigin;
		storage.currentSlotIndex = 1;
	}

	void Renderer2D::endScene() {
		PROFILE_SCOPE(__FUNCSIG__);

		storage.vertexBuffer->copyData(storage.vertexBufferOrigin, sizeof(QuadVertex) * storage.quadCount * 4);

		// Flush.
		storage.vertexArray->bind();

		for (uint32 slotIndex = 0; slotIndex < storage.currentSlotIndex; slotIndex++) {
			storage.textureSlots[slotIndex]->bind(slotIndex);
		}

		RenderCommand::drawIndexed(storage.vertexArray, storage.quadCount * 6);
	}

	void Renderer2D::drawQuad(const vec2<float>& position, const vec2<float>& size, const vec4<float>& color) {
		drawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const vec3<float>& position, const vec2<float>& size, const vec4<float>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		constexpr float32 textureSlot = 0.0f;
		constexpr float32 tilingFactor = 1.0f;

		storage.vertexBufferPointer->position = position;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {0.0f, 0.0f};
		storage.vertexBufferPointer->textureSlot = textureSlot;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0] + size[0], position[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {1.0f, 0.0f};
		storage.vertexBufferPointer->textureSlot = textureSlot;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0] + size[0], position[1] + size[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {1.0f, 1.0f};
		storage.vertexBufferPointer->textureSlot = textureSlot;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0], position[1] + size[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {0.0f, 1.0f};
		storage.vertexBufferPointer->textureSlot = textureSlot;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}

	void Renderer2D::drawQuad(const vec2<float>& position, const vec2<float>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const vec3<float>& position, const vec2<float>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		const vec4<float32> color = {1.0f, 1.0f, 1.0f, 1.0f};

		int32 slotIndex = 0.0f;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = (int32) i;

				break;
			}
		}

		if (slotIndex == 0) {
			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = texture;
			storage.currentSlotIndex++;
		}

		storage.vertexBufferPointer->position = position;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {0.0f, 0.0f};
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0] + size[0], position[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {1.0f, 0.0f};
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0] + size[0], position[1] + size[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {1.0f, 1.0f};
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = {position[0], position[1] + size[1], position[2]};
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = {0.0f, 1.0f};
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 rotation, const vec4<float32>& color) {
		//drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 rotation, const vec4<float32>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		/*const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::rotateZ(rotation) * mat4x4<float>::scale(scale);

		storage.colorTextureShader->bind();
		storage.colorTextureShader->setUniform("u_color", color);
		storage.colorTextureShader->setUniform("u_tilingFactor", 1.0f);
		storage.colorTextureShader->setUniform("u_transform", transform);

		storage.defaultTexture->bind(0);

		storage.vertexArray->bind();

		RenderCommand::drawIndexed(storage.vertexArray);*/
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		//drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		/*const vec3<float> scale = {size.x, size.y, 1.0f};
		const mat4x4<float> transform = mat4x4<float>::translate(position) * mat4x4<float>::rotateZ(rotation) * mat4x4<float>::scale(scale);

		storage.colorTextureShader->bind();
		storage.colorTextureShader->setUniform("u_color", tintColor);
		storage.colorTextureShader->setUniform("u_tilingFactor", tilingFactor);
		storage.colorTextureShader->setUniform("u_transform", transform);

		texture->bind(0);

		storage.vertexArray->bind();

		RenderCommand::drawIndexed(storage.vertexArray);*/
	}
}