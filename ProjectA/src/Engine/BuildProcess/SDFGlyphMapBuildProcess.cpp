#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	static constexpr uint64 MAX_GLYPH_COUNT = 2048;
	static constexpr uint64 GLYPH_SUBIMAGE_DIMENSION = 64;
	static constexpr uint64 SPREAD_FACTOR = 7;

	SDFGlyphMap SDFGlyphMapBuildProcess::build(const TTFFile& ttfFile) {
		ENGINE_ASSERT(ttfFile.glyphs.size() < MAX_GLYPH_COUNT, "The input font must have less than [] glyphs.", MAX_GLYPH_COUNT);

		SDFGlyphMap sdfGlyphMap;
		sdfGlyphMap.lineHeight = GLYPH_SUBIMAGE_DIMENSION;
		sdfGlyphMap.spreadFactor = SPREAD_FACTOR;
		sdfGlyphMap.glyphs.resize(ttfFile.glyphs.size());

		const ref<Device> device = Renderer::getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		const uint64 glyphBufferSize = computeMaxGlyphByteSize(ttfFile);

		constexpr uint64 fontBufferSize = sizeof(sint32) * 3;

		const ref<Buffer> fontStagingBuffer = Renderer::createStagingBuffer(fontBufferSize);

		sint32* fontMappedRange = fontStagingBuffer->map<sint32>(0, fontBufferSize);

		*fontMappedRange = SPREAD_FACTOR;

		fontMappedRange += 1;

		*fontMappedRange = ttfFile.hhea.ascent;

		fontMappedRange += 1;

		*fontMappedRange = ttfFile.hhea.descent;

		fontStagingBuffer->unmap();

		BufferDescriptor fontStorageBufferDescriptor{};
		fontStorageBufferDescriptor.size = fontBufferSize;
		fontStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> fontStorageBuffer = Renderer::createBuffer(fontStorageBufferDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontStorageBuffer, 0, 0, fontBufferSize);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		const ref<Buffer> glyphStagingBuffer = Renderer::createStagingBuffer(glyphBufferSize);

		BufferDescriptor glyphBufferDescriptor{};
		glyphBufferDescriptor.size = glyphBufferSize;
		glyphBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphBuffer = Renderer::createBuffer(glyphBufferDescriptor);

		constexpr uint64 glyphOffsetBufferSize = sizeof(uint32) * 3;

		const ref<Buffer> glyphOffsetStagingBuffer = Renderer::createStagingBuffer(glyphOffsetBufferSize);

		BufferDescriptor glyphOffsetBufferDescriptor{};
		glyphOffsetBufferDescriptor.size = glyphOffsetBufferSize;
		glyphOffsetBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphOffsetBuffer = Renderer::createBuffer(glyphOffsetBufferDescriptor);

		// Used for generating the distance field for each glyph.
		ImageDescriptor glyphImageDescriptor{};
		glyphImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
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

		const ref<ShaderModule> glyphImageShaderModule = Renderer::loadShaderModule("assets/shaders/GlyphImage.comp.spv", glyphImageShaderModuleDescriptor);

		ComputePipelineDescriptor glyphImagePipelineDescriptor;
		glyphImagePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImagePipelineDescriptor.computeShaderModule = glyphImageShaderModule;

		const ref<ComputePipeline> glyphImagePipeline = Renderer::createComputePipeline(glyphImagePipelineDescriptor);

		// Contains every glyph distance fields.
		ImageDescriptor glyphMapSdfImageDescriptor{};
		glyphMapSdfImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapSdfImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapSdfImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		glyphMapSdfImageDescriptor.width = 2048;
		glyphMapSdfImageDescriptor.height = 2048; // Can be set to 1024.
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

		const ref<ShaderModule> glyphSdfShaderModule = Renderer::loadShaderModule("assets/shaders/GlyphDistanceField.comp.spv", glyphSdfShaderModuleDescriptor);

		ComputePipelineDescriptor glyphSdfPipelineDescriptor;
		glyphSdfPipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfPipelineDescriptor.computeShaderModule = glyphSdfShaderModule;

		const ref<ComputePipeline> glyphSdfPipeline = Renderer::createComputePipeline(glyphSdfPipelineDescriptor);

		transitionImage(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL);
		transitionImage(sdfGlyphMap.glyphMap, ImageLayout::UNDEFINED, ImageLayout::GENERAL);

		const uint32 lineHeight = ttfFile.hhea.ascent - ttfFile.hhea.descent;

		uint32 xOffset = 0;
		uint32 lineIndex = 0;

		// Build process
		{
			for (uint16 glyphIndex = 0; glyphIndex < 64; glyphIndex += 1) {
				const Glyph& glyph = ttfFile.glyphs[glyphIndex];
				const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];

				SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

				sdfGlyph.advanceWidth = static_cast<float32>(hMetric.advanceWidth);
				sdfGlyph.advanceWidth /= lineHeight + GLYPH_SUBIMAGE_DIMENSION * SPREAD_FACTOR;
				sdfGlyph.advanceWidth *= GLYPH_SUBIMAGE_DIMENSION;
				sdfGlyph.advanceWidth = std::round(sdfGlyph.advanceWidth);

				if (glyph.edges.empty()) {
					sdfGlyph.invisible = true;

					continue;
				}

				const uint64 glyphSize = sizeof(sint32) * 6 + sizeof(GlyphEdge) * glyph.edges.size();

				GlyphBuffer glyphBufferData{};
				glyphBufferData.minCoordinateX = glyph.min.x;
				glyphBufferData.minCoordinateY = glyph.min.y;
				glyphBufferData.maxCoordinateX = glyph.max.x;
				glyphBufferData.maxCoordinateY = glyph.max.y;
				glyphBufferData.advanceWidth = hMetric.advanceWidth;
				glyphBufferData.leftBearing = hMetric.leftSideBearing;
				glyphBufferData.edges.resize(glyph.edges.size());

				for (uint16 edgeIndex = 0; edgeIndex < glyph.edges.size(); edgeIndex += 1) {
					const Edge& edge = glyph.edges[edgeIndex];
					GlyphEdge& glyphBufferEdge = glyphBufferData.edges[edgeIndex];

					glyphBufferEdge.startX = edge.start.x;
					glyphBufferEdge.startY = edge.start.y;
					glyphBufferEdge.endX = edge.end.x;
					glyphBufferEdge.endY = edge.end.y;
					glyphBufferEdge.controlX = edge.control.x;
					glyphBufferEdge.controlY = edge.control.y;
					glyphBufferEdge.curved = edge.curved;
				}

				// TODO: Use persistent mapping for this buffer.
				sint32* glyphMappedRange = glyphStagingBuffer->map<sint32>(0, glyphBufferSize);

				sint32* glyphMappedRangeCopy = glyphMappedRange;

				std::copy_n(&glyphBufferData.minCoordinateX, 6, glyphMappedRangeCopy);

				glyphMappedRangeCopy += 6;

				std::copy(glyphBufferData.edges.begin(), glyphBufferData.edges.end(), (GlyphEdge*) glyphMappedRangeCopy);

				glyphMappedRangeCopy += 8 * glyphBufferData.edges.size();

				std::fill_n(glyphMappedRangeCopy, glyphBufferSize - (glyphMappedRangeCopy - glyphMappedRange), 0);

				glyphStagingBuffer->unmap();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				{
					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(glyphStagingBuffer, glyphBuffer, 0, 0, glyphBufferSize);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				{
					const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->bindComputePipeline(glyphImagePipeline);
					commandEncoder->bindDescriptorSets(0);
					commandEncoder->dispatchGroups(128, 128, 1);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				// Width of the glyph sub-image, with left and right spreads included.
				float32 glyphWidth = static_cast<float32>(glyph.max.x - glyph.min.x);
				glyphWidth /= lineHeight + GLYPH_SUBIMAGE_DIMENSION * SPREAD_FACTOR;
				glyphWidth *= GLYPH_SUBIMAGE_DIMENSION;

				// TODO: lyah::ceil(float32)
				glyphWidth = std::round(glyphWidth);

				glyphWidth += SPREAD_FACTOR * 2;

				sdfGlyph.width = static_cast<uint32>(glyphWidth);
				sdfGlyph.xOffset = xOffset;
				sdfGlyph.lineIndex = lineIndex;

				sdfGlyph.leftSideBearing = static_cast<float32>(hMetric.leftSideBearing);
				sdfGlyph.leftSideBearing /= lineHeight + GLYPH_SUBIMAGE_DIMENSION * SPREAD_FACTOR;
				sdfGlyph.leftSideBearing *= GLYPH_SUBIMAGE_DIMENSION;
				sdfGlyph.leftSideBearing = std::round(sdfGlyph.leftSideBearing);

				sdfGlyph.invisible = false;

				if (xOffset + sdfGlyph.width >= sdfGlyphMap.glyphMap->getWidth()) {
					xOffset = 0;
					lineIndex += 1;
				}

				// TODO: Use persistent mapping for this buffer.
				uint32* glyphOffsetMappedRange = glyphOffsetStagingBuffer->map<uint32>(0, glyphOffsetBufferSize);

				glyphOffsetMappedRange[0] = xOffset;
				glyphOffsetMappedRange[1] = lineIndex * GLYPH_SUBIMAGE_DIMENSION;
				glyphOffsetMappedRange[2] = SPREAD_FACTOR;

				glyphOffsetStagingBuffer->unmap();

				{
					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(glyphOffsetStagingBuffer, glyphOffsetBuffer, 0, 0, glyphOffsetBufferSize);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				{
					const ref<CommandEncoder> commandEncoder = device->getComputeQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->bindComputePipeline(glyphSdfPipeline);
					commandEncoder->bindDescriptorSets(0);
					commandEncoder->dispatchGroups(8, 8, 1);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					device->getComputeQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
					device->waitIdle();
				}

				xOffset += sdfGlyph.width;
			}
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

		return sizeof(sint32) * 6 + sizeof(GlyphEdge) * maxGlyphEdgeCount;
	}
}