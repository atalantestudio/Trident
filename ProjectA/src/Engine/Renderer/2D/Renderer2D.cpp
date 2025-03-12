#include "pch.hpp"

#include "Engine/Filesystem/FileLoader.hpp"
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	const OrthographicCamera3D* Renderer2D::camera = nullptr;

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

		// TODO: Renderer capabilities class
		static constexpr uint32 MAX_TEXTURE_SLOTS = 32;

		ref<PipelineLayout> pipelineLayout;
		ref<RenderPipeline> renderPipeline;
		ref<Buffer> vertexBuffer;
		uint32 quadCount = 0;
		QuadVertex* vertexBufferOrigin = nullptr;
		QuadVertex* vertexBufferPointer = nullptr;

		// TODO: Replace with assets/asset containers
		std::array<ref<Texture2D>, MAX_TEXTURE_SLOTS> textureSlots;
		uint32 currentSlotIndex = 1; // Skip default texture

		vec4<float32> quadVertexPositions[4];
		vec2<float32> quadVertexTextureCoordinates[4];

		Renderer2D::Statistics statistics;
	};

	static Renderer2DStorage storage;

	void Renderer2D::construct() {
		PROFILE_SCOPE(FUNC_SIG);

		DescriptorSetLayoutBinding cameraDescriptorSetLayoutBinding{};
		cameraDescriptorSetLayoutBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		//cameraDescriptorSetLayoutBinding.descriptorCount = 1;
		cameraDescriptorSetLayoutBinding.stageFlags = ShaderStage::VERTEX;

		DescriptorSetLayoutBinding texturesDescriptorSetLayoutBinding{};
		texturesDescriptorSetLayoutBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		//texturesDescriptorSetLayoutBinding.descriptorCount = 32;
		texturesDescriptorSetLayoutBinding.stageFlags = ShaderStage::FRAGMENT;

		DescriptorSetLayout cameraDescriptorSetLayout;
		cameraDescriptorSetLayout.bindings[0] = cameraDescriptorSetLayoutBinding;

		DescriptorSetLayout texturesDescriptorSetLayout;
		texturesDescriptorSetLayout.bindings[0] = texturesDescriptorSetLayoutBinding;

		PipelineLayoutDescriptor pipelineLayoutDescriptor;
		pipelineLayoutDescriptor.layouts[0] = cameraDescriptorSetLayout;
		pipelineLayoutDescriptor.layouts[1] = texturesDescriptorSetLayout;

		//storage.pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		RenderPipelineDescriptor renderPipelineDescriptor;
		renderPipelineDescriptor.vertexBufferLayout.add<vec3<float32>>(0, false);	// a_position
		renderPipelineDescriptor.vertexBufferLayout.add<vec4<float32>>(0, false);	// a_color
		renderPipelineDescriptor.vertexBufferLayout.add<vec2<float32>>(0, false);	// a_uv
		renderPipelineDescriptor.vertexBufferLayout.add<int32>(0, false);			// a_texture
		renderPipelineDescriptor.vertexBufferLayout.add<float32>(0, false);			// a_tilingFactor

		renderPipelineDescriptor.pipelineLayout = storage.pipelineLayout;

		if (Renderer::getGraphicsApi() == GraphicsAPI::OPENGL) {
			const std::string colorTextureVertexShaderCode = FileLoader::load("assets/shaders/ColorTexture.vert");
			const std::string colorTextureFragmentShaderCode = FileLoader::load("assets/shaders/ColorTexture.frag");

			//renderPipelineDescriptor.vertexShaderModule = Renderer::createShaderModule(ShaderStage::VERTEX, colorTextureVertexShaderCode);
			//renderPipelineDescriptor.fragmentShaderModule = Renderer::createShaderModule(ShaderStage::FRAGMENT, colorTextureFragmentShaderCode);
		}
		else if (Renderer::getGraphicsApi() == GraphicsAPI::VULKAN) {
			const std::string colorTextureVertexShaderCode = FileLoader::load("assets/shaders/ColorTexture.vert.spv");
			const std::string colorTextureFragmentShaderCode = FileLoader::load("assets/shaders/ColorTexture.frag.spv");

			//renderPipelineDescriptor.vertexShaderModule = Renderer::createShaderModule(ShaderStage::VERTEX, colorTextureVertexShaderCode);
			//renderPipelineDescriptor.fragmentShaderModule = Renderer::createShaderModule(ShaderStage::FRAGMENT, colorTextureFragmentShaderCode);
		}

		renderPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;

		//storage.renderPipeline = Renderer::createRenderPipeline(renderPipelineDescriptor);
		//storage.renderPipeline->bind();

		BufferDescriptor vertexBufferDescriptor{};
		vertexBufferDescriptor.size = sizeof(QuadVertex) * Renderer2DStorage::MAX_VERTICES;
		vertexBufferDescriptor.usage = BufferUsage::VERTEX;

		//storage.vertexBuffer = Renderer::createBuffer(vertexBufferDescriptor);
		//storage.vertexBuffer->setLayout(renderPipelineDescriptor.vertexBufferLayout);

		//storage.renderPipeline->addVertexBuffer(storage.vertexBuffer);

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

		BufferDescriptor indexBufferDescriptor{};
		indexBufferDescriptor.size = Renderer2DStorage::MAX_INDICES;
		indexBufferDescriptor.usage = BufferUsage::INDEX;

		//const ref<Buffer> indexBuffer = Renderer::createBuffer(indexBufferDescriptor);

		//indexBuffer->write(static_cast<const void*>(indices));

		delete[] indices;

		//storage.renderPipeline->setIndexBuffer(indexBuffer);

		const uint32 defaultTextureData = 0xffffffff;

		storage.textureSlots[0] = Texture2D::create(1, 1);
		storage.textureSlots[0]->setData(&defaultTextureData, 4);

		int32 textureSamplers[Renderer2DStorage::MAX_TEXTURE_SLOTS]{};

		for (int32 i = 0; i < Renderer2DStorage::MAX_TEXTURE_SLOTS; i++) {
			textureSamplers[i] = i;
		}

		//storage.renderPipeline->setUniform("u_textures", textureSamplers, Renderer2DStorage::MAX_TEXTURE_SLOTS);

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

	void Renderer2D::beginScene() {
		PROFILE_SCOPE(FUNC_SIG);

		//storage.renderPipeline->bind();
		//storage.renderPipeline->setUniform("u_projectionView", camera->projectionView);

		storage.quadCount = 0;
		storage.vertexBufferPointer = storage.vertexBufferOrigin;
		storage.currentSlotIndex = 1;
	}

	void Renderer2D::endScene() {
		PROFILE_SCOPE(FUNC_SIG);

		if (storage.quadCount == 0) {
			return;
		}

		//storage.vertexBuffer->copyData(storage.vertexBufferOrigin, sizeof(QuadVertex) * storage.quadCount * 4);

		//storage.renderPipeline->bind();

		for (uint32 slotIndex = 0; slotIndex < storage.currentSlotIndex; slotIndex++) {
			storage.textureSlots[slotIndex]->bind(slotIndex);
		}

		//Renderer::getRendererApi()->drawIndexed(storage.renderPipeline, storage.quadCount * 6);

		storage.statistics.drawCallCount++;
	}

	void Renderer2D::startNewBatch() {
		endScene();

		storage.quadCount = 0;
		storage.vertexBufferPointer = storage.vertexBufferOrigin;
		storage.currentSlotIndex = 1;
	}

	void Renderer2D::drawQuad(const vec2<float32>& position, const vec2<float32>& size, const vec4<float32>& color) {
		drawQuad({position[0], position[1], 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const vec3<float32>& position, const vec2<float32>& size, const vec4<float32>& color) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;
		constexpr uint32 textureIndex = 0;
		constexpr float32 tilingFactor = 1.0f;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = color;
			storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[i];
			storage.vertexBufferPointer->textureSlot = textureIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	void Renderer2D::drawQuad(const vec2<float32>& position, const vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawQuad({position[0], position[1], 0.0f}, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const vec3<float32>& position, const vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			if (storage.currentSlotIndex >= storage.MAX_TEXTURE_SLOTS) {
				startNewBatch();
			}

			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = texture;
			storage.currentSlotIndex++;
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = tintColor;
			storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[i];
			storage.vertexBufferPointer->textureSlot = slotIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	void Renderer2D::drawQuad(const vec2<float32>& position, const vec2<float32>& size, const ref<Subtexture>& subtexture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawQuad({position[0], position[1], 0.0f}, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const vec3<float32>& position, const vec2<float32>& size, const ref<Subtexture>& subtexture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*subtexture->texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			if (storage.currentSlotIndex >= storage.MAX_TEXTURE_SLOTS) {
				startNewBatch();
			}

			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = subtexture->texture;
			storage.currentSlotIndex++;
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = tintColor;
			storage.vertexBufferPointer->uv = subtexture->coordinates[i];
			storage.vertexBufferPointer->textureSlot = slotIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const vec4<float32>& color) {
		drawRotatedQuad({position[0], position[1], 0.0f}, size, angle, color);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const vec4<float32>& color) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;
		constexpr uint32 textureIndex = 0;
		constexpr float32 tilingFactor = 1.0f;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::rotation({0.0f, 0.0f, -1.0f}, angle);
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = color;
			storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[i];
			storage.vertexBufferPointer->textureSlot = textureIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawRotatedQuad({position[0], position[1], 0.0f}, size, angle, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const ref<Texture2D>& texture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			if (storage.currentSlotIndex >= storage.MAX_TEXTURE_SLOTS) {
				startNewBatch();
			}

			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = texture;
			storage.currentSlotIndex++;
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::rotation({0.0f, 0.0f, -1.0f}, angle);
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = tintColor;
			storage.vertexBufferPointer->uv = storage.quadVertexTextureCoordinates[i];
			storage.vertexBufferPointer->textureSlot = slotIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const ref<Subtexture>& subtexture, float32 tilingFactor, const vec4<float32>& tintColor) {
		drawRotatedQuad({position[0], position[1], 0.0f}, size, angle, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const ref<Subtexture>& subtexture, float32 tilingFactor, const vec4<float32>& tintColor) {
		PROFILE_SCOPE(FUNC_SIG);

		constexpr uint32 quadVertexCount = 4;

		if (storage.quadCount >= storage.MAX_QUADS) {
			startNewBatch();
		}

		uint32 slotIndex = 0;

		for (uint32 i = 0; i < storage.currentSlotIndex; i++) {
			if (*subtexture->texture.get() == *storage.textureSlots[i].get()) {
				slotIndex = i;

				break;
			}
		}

		if (slotIndex == 0) {
			if (storage.currentSlotIndex >= storage.MAX_TEXTURE_SLOTS) {
				startNewBatch();
			}

			// Reserve new texture slot.
			slotIndex = storage.currentSlotIndex;

			storage.textureSlots[storage.currentSlotIndex] = subtexture->texture;
			storage.currentSlotIndex++;
		}

		mat4x4<float32> transform = mat4x4<float32>::scaling({size[0], size[1], 1.0f});
		transform *= mat4x4<float32>::rotation({0.0f, 0.0f, -1.0f}, angle);
		transform *= mat4x4<float32>::translation(position);

		for (uint32 i = 0; i < quadVertexCount; i++) {
			storage.vertexBufferPointer->position = storage.quadVertexPositions[i] * transform;
			storage.vertexBufferPointer->color = tintColor;
			storage.vertexBufferPointer->uv = subtexture->coordinates[i];
			storage.vertexBufferPointer->textureSlot = slotIndex;
			storage.vertexBufferPointer->tilingFactor = tilingFactor;
			storage.vertexBufferPointer++;
		}

		storage.quadCount++;

		storage.statistics.quadCount++;
	}

	Renderer2D::Statistics& Renderer2D::getStatistics() {
		return storage.statistics;
	}

	void Renderer2D::resetStatistics() {
		memset(&storage.statistics, 0, sizeof(Statistics));
	}
}