#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	static constexpr uint64 MAX_GLYPH_COUNT = 2048;
	static constexpr uint64 SPREAD_FACTOR = 7;

	ref<Image> SDFGlyphMapBuildProcess::build(const TTFFile& ttfFile) {
		ENGINE_ASSERT(ttfFile.glyphs.size() < MAX_GLYPH_COUNT, "The input font must have less than [] glyphs.", MAX_GLYPH_COUNT);

		const ref<Device> device = Renderer::getBackend()->getDevice();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		const uint64 maxGlyphSize = computeMaxGlyphByteSize(ttfFile);

		BufferDescriptor fontStagingBufferDescriptor{};
		fontStagingBufferDescriptor.size = sizeof(sint32) * 3;
		fontStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> fontStagingBuffer = Renderer::getBackend()->createBuffer(fontStagingBufferDescriptor);

		sint32* fontMappedRange = fontStagingBuffer->map<sint32>(0, fontStagingBufferDescriptor.size);

		const uint32 lineHeight = ttfFile.hhea.ascent - ttfFile.hhea.descent;

		*fontMappedRange = SPREAD_FACTOR;

		fontMappedRange += 1;

		*fontMappedRange = ttfFile.hhea.ascent;

		fontMappedRange += 1;

		*fontMappedRange = ttfFile.hhea.descent;

		fontStagingBuffer->unmap();

		BufferDescriptor fontStorageBufferDescriptor{};
		fontStorageBufferDescriptor.size = fontStagingBufferDescriptor.size;
		fontStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> fontStorageBuffer = Renderer::getBackend()->createBuffer(fontStorageBufferDescriptor);

		{
			const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

			commandEncoder->begin();
			commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontStorageBuffer, 0, 0, fontStagingBufferDescriptor.size);

			const ref<CommandBuffer> commandBuffer = commandEncoder->end();

			device->getGraphicsQueue()->submit(commandBuffer, nullptr, nullptr, nullptr);
			device->waitIdle();
		}

		BufferDescriptor glyphStagingBufferDescriptor{};
		glyphStagingBufferDescriptor.size = maxGlyphSize;
		glyphStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> glyphStagingBuffer = Renderer::getBackend()->createBuffer(glyphStagingBufferDescriptor);

		BufferDescriptor glyphStorageBufferDescriptor{};
		glyphStorageBufferDescriptor.size = maxGlyphSize;
		glyphStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphStorageBuffer = Renderer::getBackend()->createBuffer(glyphStorageBufferDescriptor);

		BufferDescriptor glyphOffsetStagingBufferDescriptor{};
		glyphOffsetStagingBufferDescriptor.size = sizeof(uint32) * 3;
		glyphOffsetStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> glyphOffsetStagingBuffer = Renderer::getBackend()->createBuffer(glyphOffsetStagingBufferDescriptor);

		BufferDescriptor glyphOffsetStorageBufferDescriptor{};
		glyphOffsetStorageBufferDescriptor.size = glyphOffsetStagingBufferDescriptor.size;
		glyphOffsetStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphOffsetStorageBuffer = Renderer::getBackend()->createBuffer(glyphOffsetStorageBufferDescriptor);

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

		const ref<Image> glyphImage = Renderer::getBackend()->createImage(glyphImageDescriptor);

		const ImageViewDescriptor glyphImageViewDescriptor;

		const ref<ImageView> glyphImageView = glyphImage->createView(glyphImageViewDescriptor);

		DescriptorSetLayoutBinding fontBufferDescriptorSetLayoutBinding;
		fontBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		fontBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		fontBufferDescriptorSetLayoutBinding.buffers = {fontStorageBuffer};

		DescriptorSetLayoutBinding glyphBufferDescriptorSetLayoutBinding;
		glyphBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphBufferDescriptorSetLayoutBinding.buffers = {glyphStorageBuffer};

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

		const ref<PipelineLayout> glyphImagePipelineLayout = Renderer::getBackend()->createPipelineLayout(glyphImagePipelineLayoutDescriptor);

		ShaderModuleDescriptor glyphImageShaderModuleDescriptor{};
		glyphImageShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		glyphImageShaderModuleDescriptor.entryPoint = "main";

		const chain<uint8> glyphImageShaderModuleCode = Filesystem::loadByteBuffer("assets/shaders/GlyphImage.comp.spv");

		const ref<ShaderModule> glyphImageShaderModule = Renderer::getBackend()->createShaderModule(glyphImageShaderModuleDescriptor, glyphImageShaderModuleCode);

		ComputePipelineDescriptor glyphImagePipelineDescriptor;
		glyphImagePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImagePipelineDescriptor.computeShaderModule = glyphImageShaderModule;

		const ref<ComputePipeline> glyphImagePipeline = Renderer::getBackend()->createComputePipeline(glyphImagePipelineDescriptor);

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

		const ref<Image> glyphMapSdfImage = Renderer::getBackend()->createImage(glyphMapSdfImageDescriptor);

		const ImageViewDescriptor glyphMapSdfImageViewDescriptor;

		const ref<ImageView> glyphMapSdfImageView = glyphMapSdfImage->createView(glyphMapSdfImageViewDescriptor);

		DescriptorSetLayoutBinding glyphMapSdfImageDescriptorSetLayoutBinding;
		glyphMapSdfImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphMapSdfImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphMapSdfImageDescriptorSetLayoutBinding.imageViews = {glyphMapSdfImageView};

		DescriptorSetLayoutBinding glyphIndexDescriptorSetLayoutBinding;
		glyphIndexDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphIndexDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphIndexDescriptorSetLayoutBinding.buffers = {glyphOffsetStorageBuffer};

		DescriptorSetLayout glyphSdfDescriptorSetLayout;
		glyphSdfDescriptorSetLayout.bindings = {
			glyphImageDescriptorSetLayoutBinding,
			glyphMapSdfImageDescriptorSetLayoutBinding,
			glyphIndexDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor;
		glyphSdfPipelineLayoutDescriptor.layouts = {glyphSdfDescriptorSetLayout};

		const ref<PipelineLayout> glyphSdfPipelineLayout = Renderer::getBackend()->createPipelineLayout(glyphSdfPipelineLayoutDescriptor);

		ShaderModuleDescriptor glyphSdfShaderModuleDescriptor{};
		glyphSdfShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		glyphSdfShaderModuleDescriptor.entryPoint = "main";

		const chain<uint8> glyphSdfShaderModuleCode = Filesystem::loadByteBuffer("assets/shaders/GlyphDistanceField.comp.spv");

		const ref<ShaderModule> glyphSdfShaderModule = Renderer::getBackend()->createShaderModule(glyphSdfShaderModuleDescriptor, glyphSdfShaderModuleCode);

		ComputePipelineDescriptor glyphSdfPipelineDescriptor;
		glyphSdfPipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfPipelineDescriptor.computeShaderModule = glyphSdfShaderModule;

		const ref<ComputePipeline> glyphSdfPipeline = Renderer::getBackend()->createComputePipeline(glyphSdfPipelineDescriptor);

		transitionImage(glyphImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL);
		transitionImage(glyphMapSdfImage, ImageLayout::UNDEFINED, ImageLayout::GENERAL);

		uint32 xOffset = 0;
		uint32 yOffset = 0;

		// Build process
		{
			for (uint16 glyphIndex = 0; glyphIndex < ttfFile.maxp.numGlyphs; glyphIndex += 1) {
				const Glyph& glyph = ttfFile.glyphs[glyphIndex];
				const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];

				if (glyph.edges.empty()) {
					continue;
				}

				const uint64 glyphSize = sizeof(sint32) * 6 + sizeof(GlyphEdge) * glyph.edges.size();

				GlyphBuffer glyphBuffer{};
				glyphBuffer.minCoordinateX = glyph.min.x;
				glyphBuffer.minCoordinateY = glyph.min.y;
				glyphBuffer.maxCoordinateX = glyph.max.x;
				glyphBuffer.maxCoordinateY = glyph.max.y;
				glyphBuffer.advanceWidth = hMetric.advanceWidth;
				glyphBuffer.leftBearing = hMetric.leftSideBearing;
				glyphBuffer.edges.resize(glyph.edges.size());

				for (uint16 edgeIndex = 0; edgeIndex < glyph.edges.size(); edgeIndex += 1) {
					const Edge& edge = glyph.edges[edgeIndex];
					GlyphEdge& glyphBufferEdge = glyphBuffer.edges[edgeIndex];

					glyphBufferEdge.startX = edge.start.x;
					glyphBufferEdge.startY = edge.start.y;
					glyphBufferEdge.endX = edge.end.x;
					glyphBufferEdge.endY = edge.end.y;
					glyphBufferEdge.controlX = edge.control.x;
					glyphBufferEdge.controlY = edge.control.y;
					glyphBufferEdge.curved = edge.curved;
				}

				// TODO: Use persistent mapping for this buffer.
				sint32* glyphMappedRange = glyphStagingBuffer->map<sint32>(0, maxGlyphSize);

				sint32* glyphMappedRangeCopy = glyphMappedRange;

				std::copy_n(&glyphBuffer.minCoordinateX, 6, glyphMappedRangeCopy);

				glyphMappedRangeCopy += 6;

				std::copy(glyphBuffer.edges.begin(), glyphBuffer.edges.end(), (GlyphEdge*) glyphMappedRangeCopy);

				glyphMappedRangeCopy += 8 * glyphBuffer.edges.size();

				std::fill_n(glyphMappedRangeCopy, maxGlyphSize - (glyphMappedRangeCopy - glyphMappedRange), 0);

				glyphStagingBuffer->unmap();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				{
					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(glyphStagingBuffer, glyphStorageBuffer, 0, 0, maxGlyphSize);

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

				float32 glyphWidth = glyph.max.x - glyph.min.x;
				glyphWidth /= lineHeight + 64 * SPREAD_FACTOR;
				glyphWidth *= 64;

				// TODO: lyah::ceil(float32)
				glyphWidth = std::round(glyphWidth);

				glyphWidth += SPREAD_FACTOR * 2;

				if (xOffset + glyphWidth >= glyphMapSdfImage->getWidth()) {
					xOffset = 0;
					yOffset += 64;
				}

				// TODO: Use persistent mapping for this buffer.
				uint32* glyphOffsetMappedRange = glyphOffsetStagingBuffer->map<uint32>(0, glyphOffsetStagingBufferDescriptor.size);

				*glyphOffsetMappedRange = xOffset;

				glyphOffsetMappedRange += 1;

				*glyphOffsetMappedRange = yOffset;

				glyphOffsetMappedRange += 1;

				*glyphOffsetMappedRange = SPREAD_FACTOR;

				glyphOffsetStagingBuffer->unmap();

				{
					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(glyphOffsetStagingBuffer, glyphOffsetStorageBuffer, 0, 0, glyphOffsetStagingBufferDescriptor.size);

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

				xOffset += glyphWidth;
			}
		}

		transitionImage(glyphMapSdfImage, ImageLayout::GENERAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

		return glyphMapSdfImage;
	}

	void SDFGlyphMapBuildProcess::transitionImage(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout) {
		const ref<Device> device = Renderer::getBackend()->getDevice();

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