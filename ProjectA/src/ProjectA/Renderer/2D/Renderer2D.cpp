#include "pch.hpp"

#include "ProjectA/Renderer/2D/Renderer2D.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/Shader.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	struct QuadVertex {
		vec_sts<3> position;
		vec_sts<4> color;
		vec_sts<2> uv;
		uint32 textureSlot = 0;
		float32 tilingFactor = 0.0f;
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

		lyah::vec4<float32> quadVertexPositions[4];
		lyah::vec2<float32> quadVertexTextureCoordinates[4];
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

		storage.quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
		storage.quadVertexPositions[1] = {-0.5f,  0.5f, 0.0f, 1.0f};
		storage.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
		storage.quadVertexPositions[3] = { 0.5f, -0.5f, 0.0f, 1.0f};

		storage.quadVertexTextureCoordinates[0] = {0.0f, 0.0f};
		storage.quadVertexTextureCoordinates[1] = {0.0f, 1.0f};
		storage.quadVertexTextureCoordinates[2] = {1.0f, 1.0f};
		storage.quadVertexTextureCoordinates[3] = {1.0f, 0.0f};
	}

	void Renderer2D::destroy() {
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

		storage.vertexArray->bind();

		for (uint32 slotIndex = 0; slotIndex < storage.currentSlotIndex; slotIndex++) {
			storage.textureSlots[slotIndex]->bind(slotIndex);
		}

		RenderCommand::drawIndexed(storage.vertexArray, storage.quadCount * 6);
	}

	void Renderer2D::drawQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, const lyah::vec4<float32>& color) {
		drawQuad({position[0], position[1], 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, const lyah::vec4<float32>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		constexpr uint32 textureIndex = 0;
		constexpr float32 tilingFactor = 1.0f;

		lyah::mat4x4<float32> transform = lyah::mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= lyah::mat4x4<float32>::translation(position);

		storage.vertexBufferPointer->position = storage.quadVertexPositions[0] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[0];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[1] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[1];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[2] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[2];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[3] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[3];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}

	void Renderer2D::drawQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor, const lyah::vec4<float32>& tintColor) {
		drawQuad({position[0], position[1], 0.0f}, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor, const lyah::vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		const lyah::vec4<float32> color = {1.0f, 1.0f, 1.0f, 1.0f};

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = texture;
			storage.currentSlotIndex++;
		}

		lyah::mat4x4<float32> transform = lyah::mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= lyah::mat4x4<float32>::translation(position);

		storage.vertexBufferPointer->position = storage.quadVertexPositions[0] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[0];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[1] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[1];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[2] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[2];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[3] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[3];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, float32 angle, const lyah::vec4<float32>& color) {
		drawRotatedQuad({position[0], position[1], 0.0f}, size, angle, color);
	}

	void Renderer2D::drawRotatedQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, float32 angle, const lyah::vec4<float32>& color) {
		PROFILE_SCOPE(__FUNCSIG__);

		constexpr uint32 textureIndex = 0;
		constexpr float32 tilingFactor = 1.0f;

		lyah::mat4x4<float32> transform = lyah::mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= lyah::mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, angle);
		transform *= lyah::mat4x4<float32>::translation(position);

		storage.vertexBufferPointer->position = storage.quadVertexPositions[0] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[0];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[1] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[1];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[2] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[2];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[3] * transform;
		storage.vertexBufferPointer->color = color;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[3];
		storage.vertexBufferPointer->textureSlot = textureIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, float32 angle, const ref<Texture2D>& texture, float32 tilingFactor, const lyah::vec4<float32>& tintColor) {
		drawRotatedQuad({position[0], position[1], 0.0f}, size, angle, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, float32 angle, const ref<Texture2D>& texture, float32 tilingFactor, const lyah::vec4<float32>& tintColor) {
		PROFILE_SCOPE(__FUNCSIG__);

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = texture;
			storage.currentSlotIndex++;
		}

		lyah::mat4x4<float32> transform = lyah::mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= lyah::mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, angle);
		transform *= lyah::mat4x4<float32>::translation(position);

		storage.vertexBufferPointer->position = storage.quadVertexPositions[0] * transform;
		storage.vertexBufferPointer->color = tintColor;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[0];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[1] * transform;
		storage.vertexBufferPointer->color = tintColor;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[1];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[2] * transform;
		storage.vertexBufferPointer->color = tintColor;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[2];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.vertexBufferPointer->position = storage.quadVertexPositions[3] * transform;
		storage.vertexBufferPointer->color = tintColor;
		storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[3];
		storage.vertexBufferPointer->textureSlot = slotIndex;
		storage.vertexBufferPointer->tilingFactor = tilingFactor;
		storage.vertexBufferPointer++;

		storage.quadCount++;
	}
}