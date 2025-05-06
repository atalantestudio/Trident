#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	static constexpr uint64 MAX_GLYPH_COUNT = 2048;
	static constexpr uint64 GLYPH_SUBIMAGE_DIMENSION = 64;
	static constexpr uint32 SPREAD_FACTOR = 0;

	SDFGlyphMap SDFGlyphMapBuildProcess::build(const TTFFile& ttfFile) {
		ENGINE_ASSERT(ttfFile.glyphs.size() < MAX_GLYPH_COUNT, "The input font must have less than [] glyphs.", MAX_GLYPH_COUNT);

		SDFGlyphMap sdfGlyphMap;
		sdfGlyphMap.spreadFactor = SPREAD_FACTOR;
		sdfGlyphMap.glyphs.resize(ttfFile.glyphs.size());

		const ref<Device> device = Renderer::getDevice();

		const uint64 glyphBufferSize = computeMaxGlyphByteSize(ttfFile);

		constexpr uint64 fontBufferSize = 3 * sizeof(sint32);

		const ref<Buffer> fontStagingBuffer = Renderer::createStagingBuffer(fontBufferSize);

		sint32* fontMappedRange = fontStagingBuffer->map<sint32>(0, fontBufferSize);

		fontMappedRange[0] = SPREAD_FACTOR;
		fontMappedRange[1] = ttfFile.hhea.ascent;
		fontMappedRange[2] = ttfFile.hhea.descent;

		fontStagingBuffer->unmap();

		BufferDescriptor fontStorageBufferDescriptor{};
		fontStorageBufferDescriptor.size = fontBufferSize;
		fontStorageBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;

		const ref<Buffer> fontStorageBuffer = Renderer::createBuffer(fontStorageBufferDescriptor);

		{
			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontStorageBuffer, 0, 0, fontBufferSize);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		const ref<Buffer> glyphStagingBuffer = Renderer::createStagingBuffer(glyphBufferSize, true);

		GlyphBuffer* glyphMappedRange = glyphStagingBuffer->map<GlyphBuffer>(0, glyphBufferSize);

		BufferDescriptor glyphBufferDescriptor{};
		glyphBufferDescriptor.size = glyphBufferSize;
		glyphBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;

		const ref<Buffer> glyphBuffer = Renderer::createBuffer(glyphBufferDescriptor);

		constexpr uint64 glyphOffsetBufferSize = 3 * sizeof(float32);

		const ref<Buffer> glyphOffsetStagingBuffer = Renderer::createStagingBuffer(glyphOffsetBufferSize, true);

		uint32* glyphOffsetMappedRange = glyphOffsetStagingBuffer->map<uint32>(0, glyphOffsetBufferSize);

		BufferDescriptor glyphOffsetBufferDescriptor{};
		glyphOffsetBufferDescriptor.size = glyphOffsetBufferSize;
		glyphOffsetBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;

		const ref<Buffer> glyphOffsetBuffer = Renderer::createBuffer(glyphOffsetBufferDescriptor);

		// Used for generating the distance field for each glyph.
		ImageDescriptor glyphImageDescriptor{};
		glyphImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
		glyphImageDescriptor.width = 4096;
		glyphImageDescriptor.height = 4096;
		glyphImageDescriptor.depth = 1;
		glyphImageDescriptor.mipLevelCount = 1;
		glyphImageDescriptor.arrayLayerCount = 1;
		glyphImageDescriptor.sampleCount = 1;

		const ref<Image> glyphImage = Renderer::createImage(glyphImageDescriptor);

		const ImageViewDescriptor glyphImageViewDescriptor;

		const ref<ImageView> glyphImageView = glyphImage->createView(glyphImageViewDescriptor);

		DescriptorSetLayoutBinding fontBufferDescriptorSetLayoutBinding;
		fontBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		fontBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		fontBufferDescriptorSetLayoutBinding.buffers = {fontStorageBuffer};

		DescriptorSetLayoutBinding glyphBufferDescriptorSetLayoutBinding;
		glyphBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphBufferDescriptorSetLayoutBinding.buffers = {glyphBuffer};

		DescriptorSetLayoutBinding glyphImageDescriptorSetLayoutBinding;
		glyphImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphImageDescriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayout glyphImageDescriptorSetLayout;
		glyphImageDescriptorSetLayout.bindings = {
			fontBufferDescriptorSetLayoutBinding,
			glyphBufferDescriptorSetLayoutBinding,
			glyphImageDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor glyphImagePipelineLayoutDescriptor;
		glyphImagePipelineLayoutDescriptor.layouts = {glyphImageDescriptorSetLayout};

		const ref<PipelineLayout> glyphImagePipelineLayout = Renderer::createPipelineLayout(glyphImagePipelineLayoutDescriptor);

		ShaderModuleDescriptor glyphImageShaderModuleDescriptor{};
		glyphImageShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		glyphImageShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> glyphImageShaderModule = Renderer::loadShaderModule("../ProjectA/src/Engine/Shader/GlyphImage.comp.spv", glyphImageShaderModuleDescriptor);

		ComputePipelineDescriptor glyphImagePipelineDescriptor;
		glyphImagePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImagePipelineDescriptor.computeShaderModule = glyphImageShaderModule;

		const ref<ComputePipeline> glyphImagePipeline = Renderer::createComputePipeline(glyphImagePipelineDescriptor);

		// Contains every glyph distance fields.
		ImageDescriptor glyphMapSdfImageDescriptor{};
		glyphMapSdfImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapSdfImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapSdfImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::SAMPLED;
		glyphMapSdfImageDescriptor.width = 2056;
		glyphMapSdfImageDescriptor.height = 2056; // Can be set to 1024.
		glyphMapSdfImageDescriptor.depth = 1;
		glyphMapSdfImageDescriptor.mipLevelCount = 1;
		glyphMapSdfImageDescriptor.arrayLayerCount = 1;
		glyphMapSdfImageDescriptor.sampleCount = 1;

		sdfGlyphMap.glyphMap = Renderer::createImage(glyphMapSdfImageDescriptor);

		const ImageViewDescriptor glyphMapSdfImageViewDescriptor;

		const ref<ImageView> glyphMapSdfImageView = sdfGlyphMap.glyphMap->createView(glyphMapSdfImageViewDescriptor);

		DescriptorSetLayoutBinding glyphMapSdfImageDescriptorSetLayoutBinding;
		glyphMapSdfImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphMapSdfImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphMapSdfImageDescriptorSetLayoutBinding.imageViews = {glyphMapSdfImageView};

		DescriptorSetLayoutBinding glyphIndexDescriptorSetLayoutBinding;
		glyphIndexDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphIndexDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphIndexDescriptorSetLayoutBinding.buffers = {glyphOffsetBuffer};

		DescriptorSetLayout glyphSdfDescriptorSetLayout;
		glyphSdfDescriptorSetLayout.bindings = {
			glyphImageDescriptorSetLayoutBinding,
			glyphMapSdfImageDescriptorSetLayoutBinding,
			glyphIndexDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor;
		glyphSdfPipelineLayoutDescriptor.layouts = {glyphSdfDescriptorSetLayout};

		const ref<PipelineLayout> glyphSdfPipelineLayout = Renderer::createPipelineLayout(glyphSdfPipelineLayoutDescriptor);

		ShaderModuleDescriptor glyphSdfShaderModuleDescriptor{};
		glyphSdfShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		glyphSdfShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> glyphSdfShaderModule = Renderer::loadShaderModule("../ProjectA/src/Engine/Shader/GlyphDistanceField.comp.spv", glyphSdfShaderModuleDescriptor);

		ComputePipelineDescriptor glyphSdfPipelineDescriptor;
		glyphSdfPipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfPipelineDescriptor.computeShaderModule = glyphSdfShaderModule;

		const ref<ComputePipeline> glyphSdfPipeline = Renderer::createComputePipeline(glyphSdfPipelineDescriptor);

		transitionImage(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL);
		transitionImage(sdfGlyphMap.glyphMap, ImageLayout::UNDEFINED, ImageLayout::GENERAL);

		float32 xOffset = 0;
		float32 yOffset = 0;

		const float32 glyphMapWidth = static_cast<float32>(sdfGlyphMap.glyphMap->getWidth());
		const float32 glyphMapHeight = static_cast<float32>(sdfGlyphMap.glyphMap->getHeight());
		const float32 lineHeight = static_cast<float32>(ttfFile.hhea.ascent - ttfFile.hhea.descent);

		const ref<Fence> copyBufferFence = Renderer::createFence();

		device->waitForFence(copyBufferFence);

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.glyphs.size(); glyphIndex += 1) {
			const Glyph& glyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];

			SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

			if (glyph.edges.empty()) {
				sdfGlyph.invisible = true;

				continue;
			}

			sdfGlyph.invisible = false;

			const uint64 glyphMapSize = sizeof(GlyphBuffer) + glyph.edges.size() * sizeof(GlyphEdge);

			// Glyph width in FUnits.
			const uint16 glyphWidth = glyph.max.x - glyph.min.x;

			// Glyph height in FUnits.
			const uint16 glyphHeight = glyph.max.y - glyph.min.y;

			// Glyph dimension (the largest between width and height) in FUnits.
			// TODO: uint16 lyah::max(uint16, uint16)
			const uint16 glyphDimension = std::max(glyphWidth, glyphHeight);

			// Glyph dimension on the high resolution image.
			//const float32 glyphHighResolutionDimension = (float32) glyphWidth / (float32) glyphImage->getWidth() * (float32) glyphDimension;

			// Glyph signed distance field width in pixels.
			//const float32 glyphSdfWidth = static_cast<float32>(glyphWidth) / glyphDimension * GLYPH_SUBIMAGE_DIMENSION;

			glyphMappedRange->minCoordinateX = glyph.min.x;
			glyphMappedRange->minCoordinateY = glyph.min.y;
			glyphMappedRange->dimension = glyphDimension;
			glyphMappedRange->edgeCount = static_cast<uint32>(glyph.edges.size());

			GlyphEdge* glyphEdgeMappedRange = static_cast<GlyphEdge*>(static_cast<void*>(glyphMappedRange + 1));

			for (uint32 edgeIndex = 0; edgeIndex < glyph.edges.size(); edgeIndex += 1) {
				const Edge& edge = glyph.edges[edgeIndex];

				glyphEdgeMappedRange[edgeIndex].startX = edge.start.x;
				glyphEdgeMappedRange[edgeIndex].startY = edge.start.y;
				glyphEdgeMappedRange[edgeIndex].endX = edge.end.x;
				glyphEdgeMappedRange[edgeIndex].endY = edge.end.y;
				glyphEdgeMappedRange[edgeIndex].controlX = edge.control.x;
				glyphEdgeMappedRange[edgeIndex].controlY = edge.control.y;
				glyphEdgeMappedRange[edgeIndex].curved = edge.curved;
			}

			sdfGlyph.width = static_cast<float32>(glyph.max.x - glyph.min.x);
			sdfGlyph.width /= lineHeight;
			sdfGlyph.width *= GLYPH_SUBIMAGE_DIMENSION;

			if (xOffset + sdfGlyph.width >= sdfGlyphMap.glyphMap->getWidth()) {
				xOffset = 0;
				yOffset += GLYPH_SUBIMAGE_DIMENSION;
			}

			sdfGlyph.minCoordinateX = xOffset / glyphMapWidth;
			sdfGlyph.maxCoordinateX = (xOffset + sdfGlyph.width) / glyphMapWidth;

			sdfGlyph.minCoordinateY = yOffset / glyphMapHeight;
			sdfGlyph.maxCoordinateY = (yOffset + GLYPH_SUBIMAGE_DIMENSION) / glyphMapHeight;

			glyphOffsetMappedRange[0] = static_cast<uint32>(xOffset);
			glyphOffsetMappedRange[1] = static_cast<uint32>(yOffset);
			glyphOffsetMappedRange[2] = SPREAD_FACTOR;

			device->resetFence(copyBufferFence);

			{
				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(glyphStagingBuffer, glyphBuffer, 0, 0, glyphMapSize);
				commandEncoder->copyBufferToBuffer(glyphOffsetStagingBuffer, glyphOffsetBuffer, 0, 0, glyphOffsetBufferSize);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, copyBufferFence);
				device->waitIdle();
			}

			{
				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

				const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(glyphImagePipeline);
				commandEncoder->bindAllDescriptorSets();

				// TODO: Dispatch less groups depending on the size of the glyph area.
				commandEncoder->dispatchGroups(128, 128, 1);

				commandEncoder->bindComputePipeline(glyphSdfPipeline);
				commandEncoder->bindAllDescriptorSets();
				commandEncoder->dispatchGroups(8, 8, 1);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
				device->waitIdle();
			}

			xOffset += std::ceil(sdfGlyph.width) + SPREAD_FACTOR * 2;
		}

		transitionImage(sdfGlyphMap.glyphMap, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

		return sdfGlyphMap;
	}

	void SDFGlyphMapBuildProcess::transitionImage(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout) {
		const ref<Device> device = Renderer::getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(image, oldLayout, newLayout, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
		device->waitIdle();
	}

	uint64 SDFGlyphMapBuildProcess::computeMaxGlyphByteSize(const TTFFile& ttfFile) {
		uint64 maxGlyphEdgeCount = 0;

		for (const Glyph& glyph : ttfFile.glyphs) {
			const uint64 glyphEdgeCount = glyph.edges.size();

			// TODO: lyah::min(uint64, uint64).
			// TODO: lyah::max(uint64, uint64).
			if (glyphEdgeCount > maxGlyphEdgeCount) {
				maxGlyphEdgeCount = glyphEdgeCount;
			}
		}

		return sizeof(GlyphBuffer) + sizeof(GlyphEdge) * maxGlyphEdgeCount;
	}
}