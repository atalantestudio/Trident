#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "GenerateSDFGlyphMapProcess.hpp"

namespace ProjectA {
	uint64 GenerateSDFGlyphMapProcess::computeMaxGlyphByteSize(const sequence<TrueTypeFont::Glyph>& glyphs) {
		uint64 maxGlyphEdgeCount = 0;

		for (const TrueTypeFont::Glyph& glyph : glyphs) {
			maxGlyphEdgeCount = max(maxGlyphEdgeCount, glyph.edges.count);
		}

		return sizeof(GlyphBuffer) + maxGlyphEdgeCount * sizeof(GlyphEdge);
	}

	GenerateSDFGlyphMapProcess::GenerateSDFGlyphMapProcess(const File<FileFormat::TTF>& file, SDFGlyphMap& output) {
		ENGINE_ASSERT(file.glyphs.count < MAX_GLYPH_COUNT, "The input font must have less than [] glyphs.", MAX_GLYPH_COUNT);

		output.extent = GLYPH_MAP_IMAGE_EXTENT;
		output.glyphs = sequence<Glyph>(file.glyphs.count);

		const ref<Buffer> fontStagingBuffer = Renderer::createStagingBuffer(sizeof(FontStorage));

		FontStorage* fontStagingBufferMappedRange = fontStagingBuffer->map<FontStorage>(0, fontStagingBuffer->size);
		fontStagingBufferMappedRange->outputGlyphSize = GLYPH_SUBIMAGE_DIMENSION;
		fontStagingBufferMappedRange->spreadFactor = SPREAD_FACTOR;
		fontStagingBufferMappedRange->ascent = file.ascent;
		fontStagingBufferMappedRange->descent = file.descent;

		fontStagingBuffer->unmap();

		createFontBuffer();
		createGlyphBuffer(computeMaxGlyphByteSize(file.glyphs));

		createGlyphOffsetBuffer();

		createGlyphImage();
		createGlyphImageView();

		createGlyphMapImage();
		createGlyphMapImageView();

		createDescriptorSetLayoutsAndSets();

		allocateDescriptorSets();

		updateDescriptorSets();

		createGlyphImagePipelineLayout();
		createGlyphImageComputePipeline();

		createGlyphSdfPipelineLayout();
		createGlyphSdfComputePipeline();

		const ref<Device> device = Renderer::getDevice();
		const ref<Queue> computeQueue = device->getQueue(QueueFamilyCapability::COMPUTE);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		ref<CommandEncoder> commandEncoder;
		ref<CommandBuffer> commandBuffer;


		BufferCopyRegion fontBufferCopyRegion{};
		fontBufferCopyRegion.sourceOffset = 0;
		fontBufferCopyRegion.destinationOffset = 0;
		fontBufferCopyRegion.size = fontBuffer->size;

		// Copy the font staging buffer to the font storage buffer and transition the glyph and glyph map images from UNDEFINED to GENERAL.
		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontBuffer, {fontBufferCopyRegion});
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		commandBuffer = commandEncoder->end();

		computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);
		device->waitIdle();

		float32 xOffset = 0;
		float32 yOffset = 0;

		const Extent3D& glyphMapExtent = glyphMapImage->getExtent();
		const float32 lineHeight = static_cast<float32>(file.ascent - file.descent);

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);

		const ref<Fence> writeBufferFence = Renderer::createFence();

		for (uint16 glyphIndex = 0; glyphIndex < file.glyphs.count; glyphIndex += 1) {
			const TrueTypeFont::Glyph& trueTypeGlyph = file.glyphs[glyphIndex];
			const TrueTypeFont::longHorMetric& hMetric = file.hMetrics[glyphIndex];

			Glyph& glyph = output.glyphs[glyphIndex];
			glyph.width = trueTypeGlyph.max.x - trueTypeGlyph.min.x;
			glyph.advanceWidth = hMetric.advanceWidth;
			glyph.leftSideBearing = hMetric.leftSideBearing;
			glyph.whitespace = trueTypeGlyph.emptyOrUnparsed();

			if (glyph.whitespace) {
				continue;
			}

			// Glyph height in FUnits.
			const uint16 glyphHeight = trueTypeGlyph.max.y - trueTypeGlyph.min.y;

			// Glyph dimension (the largest between width and height) in FUnits.
			const uint16 glyphDimension = max(glyph.width, glyphHeight);

			// Glyph dimension on the high resolution image.
			// const float32 glyphHighResolutionDimension = (float32) glyph.width / (float32) glyphImage->getWidth() * (float32) glyphDimension;

			// Glyph signed distance field width in pixels.
			// const float32 glyphSdfWidth = static_cast<float32>(glyph.width) / glyphDimension * GLYPH_SUBIMAGE_DIMENSION;

			glyphMappedRange->minCoordinateX = trueTypeGlyph.min.x;
			glyphMappedRange->minCoordinateY = trueTypeGlyph.min.y;
			glyphMappedRange->dimension = glyphDimension;
			glyphMappedRange->edgeCount = static_cast<uint32>(trueTypeGlyph.edges.count);

			GlyphEdge* glyphEdgeMappedRange = pointer_cast<GlyphEdge*>(glyphMappedRange + 1);

			for (uint32 edgeIndex = 0; edgeIndex < trueTypeGlyph.edges.count; edgeIndex += 1) {
				const TrueTypeFont::Edge& edge = trueTypeGlyph.edges[edgeIndex];

				glyphEdgeMappedRange[edgeIndex].startX = edge.start.x;
				glyphEdgeMappedRange[edgeIndex].startY = edge.start.y;
				glyphEdgeMappedRange[edgeIndex].endX = edge.end.x;
				glyphEdgeMappedRange[edgeIndex].endY = edge.end.y;
				glyphEdgeMappedRange[edgeIndex].controlX = edge.control.x;
				glyphEdgeMappedRange[edgeIndex].controlY = edge.control.y;
				glyphEdgeMappedRange[edgeIndex].curved = edge.curved;
			}

			float32 width = static_cast<float32>(trueTypeGlyph.max.x - trueTypeGlyph.min.x);
			width /= lineHeight;
			width *= GLYPH_SUBIMAGE_DIMENSION;

			if (xOffset + width >= glyphMapExtent.width) {
				xOffset = 0;
				yOffset += GLYPH_SUBIMAGE_DIMENSION;
			}

			glyph.minCoordinate.x = xOffset / glyphMapExtent.width;
			glyph.minCoordinate.y = yOffset / glyphMapExtent.height;

			glyph.maxCoordinate.x = (xOffset + width) / glyphMapExtent.width;
			glyph.maxCoordinate.y = (yOffset + GLYPH_SUBIMAGE_DIMENSION) / glyphMapExtent.height;

			glyphOffsetMappedRange[0] = static_cast<uint32>(xOffset);
			glyphOffsetMappedRange[1] = static_cast<uint32>(yOffset);
			glyphOffsetMappedRange[2] = SPREAD_FACTOR;

			device->resetFence(writeBufferFence);

			commandEncoder->reset();
			commandEncoder->begin();
			commandEncoder->bindComputePipeline(glyphImageComputePipeline);
			commandEncoder->bindDescriptorSets(glyphImagePipelineLayout, {glyphImageDescriptorSet});

			// TODO: Dispatch less groups depending on the size of the glyph area.
			commandEncoder->dispatch(128, 128, 1);

			commandEncoder->bindComputePipeline(glyphSdfComputePipeline);
			commandEncoder->bindDescriptorSets(glyphSdfPipelineLayout, {glyphSdfDescriptorSet});

			// TODO: Support custom glyph sub-image dimension (multiply down). See GlyphDistanceField.comp.
			commandEncoder->dispatch(8, 8, 1);

			commandBuffer = commandEncoder->end();

			computeQueue->submit({commandBuffer}, nullptr, nullptr, writeBufferFence);

			device->waitIdle();

			xOffset += ceil(width) + SPREAD_FACTOR * 2;
		}

		// Transition the glyph map image to TRANSFER_SRC_OPTIMAL and copy its data to the final buffer.

		BufferDescriptor glyphMapStagingBufferDescriptor{};
		glyphMapStagingBufferDescriptor.size = output.extent.width * output.extent.height;
		glyphMapStagingBufferDescriptor.usage = BufferUsage::TRANSFER_DST;
		glyphMapStagingBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;

		const ref<Buffer> glyphMapStagingBuffer = Renderer::createBuffer(glyphMapStagingBufferDescriptor);

		BufferImageCopyRegion glyphMapCopyRegion{};
		glyphMapCopyRegion.bufferOffset = 0;
		glyphMapCopyRegion.imageOffsetX = 0;
		glyphMapCopyRegion.imageOffsetY = 0;
		glyphMapCopyRegion.imageOffsetZ = 0;
		glyphMapCopyRegion.imageExtent = glyphMapImage->getExtent();
		glyphMapCopyRegion.imageMipLevel = 0;
		glyphMapCopyRegion.imageLayerOffset = 0;
		glyphMapCopyRegion.imageLayerCount = glyphMapImage->getLayerCount();

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::TRANSFER_SRC_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, {glyphMapCopyRegion});

		commandBuffer = commandEncoder->end();

		computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();

		output.data = sequence<uint8>(glyphMapStagingBuffer->size);

		const uint8* glyphMapMappedRange = glyphMapStagingBuffer->map<uint8>(0, glyphMapStagingBuffer->size);

		std::copy_n(glyphMapMappedRange, glyphMapStagingBuffer->size, &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	void GenerateSDFGlyphMapProcess::createFontBuffer() {
		BufferDescriptor fontBufferDescriptor{};
		fontBufferDescriptor.size = sizeof(FontStorage);
		fontBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		fontBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		fontBuffer = Renderer::createBuffer(fontBufferDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphBuffer(uint64 size) {
		BufferDescriptor glyphBufferDescriptor{};
		glyphBufferDescriptor.size = size;
		glyphBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		glyphBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphBufferDescriptor.persistent = true;

		glyphBuffer = Renderer::createBuffer(glyphBufferDescriptor);

		glyphMappedRange = glyphBuffer->map<GlyphBuffer>(0, glyphBuffer->size);
	}

	void GenerateSDFGlyphMapProcess::createGlyphOffsetBuffer() {
		BufferDescriptor glyphOffsetBufferDescriptor{};
		glyphOffsetBufferDescriptor.size = 3 * sizeof(float32);
		glyphOffsetBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		glyphOffsetBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphOffsetBufferDescriptor.persistent = true;

		glyphOffsetBuffer = Renderer::createBuffer(glyphOffsetBufferDescriptor);

		glyphOffsetMappedRange = glyphOffsetBuffer->map<uint32>(0, glyphOffsetBuffer->size);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImage() {
		ImageDescriptor glyphImageDescriptor{};
		glyphImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphImageDescriptor.extent.width = 4096;
		glyphImageDescriptor.extent.height = 4096;
		glyphImageDescriptor.extent.depth = 1;
		glyphImageDescriptor.mipLevelCount = 1;
		glyphImageDescriptor.layerCount = 1;
		glyphImageDescriptor.sampleCount = 1;
		glyphImageDescriptor.usage = ImageUsage::STORAGE;
		glyphImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		glyphImage = Renderer::createImage(glyphImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageView() {
		ImageViewDescriptor glyphImageViewDescriptor{};
		glyphImageViewDescriptor.mipLevelOffset = 0;
		glyphImageViewDescriptor.mipLevelCount = glyphImage->getMipLevelCount();
		glyphImageViewDescriptor.layerOffset = 0;
		glyphImageViewDescriptor.layerCount = glyphImage->getLayerCount();

		glyphImageView = glyphImage->createView(glyphImageViewDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createDescriptorSetLayoutsAndSets() {
		DescriptorSetLayoutBinding fontBufferBinding{};
		fontBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		fontBufferBinding.descriptorCount = 1;
		fontBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphBufferBinding{};
		glyphBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphBufferBinding.descriptorCount = 1;
		glyphBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphImageBinding{};
		glyphImageBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphImageBinding.descriptorCount = 1;
		glyphImageBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphMapImageBinding{};
		glyphMapImageBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphMapImageBinding.descriptorCount = 1;
		glyphMapImageBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphOffsetBufferBinding{};
		glyphOffsetBufferBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphOffsetBufferBinding.descriptorCount = 1;
		glyphOffsetBufferBinding.shaderStages = ShaderStage::COMPUTE;

		glyphImageDescriptorSetLayout = Renderer::createDescriptorSetLayout({fontBufferBinding, glyphBufferBinding, glyphImageBinding});
		glyphSdfDescriptorSetLayout = Renderer::createDescriptorSetLayout({glyphImageBinding, glyphMapImageBinding, fontBufferBinding, glyphOffsetBufferBinding});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImagePipelineLayout() {
		PipelineLayoutDescriptor glyphImagePipelineLayoutDescriptor{};
		glyphImagePipelineLayoutDescriptor.descriptorSetLayouts = {glyphImageDescriptorSetLayout};

		glyphImagePipelineLayout = Renderer::createPipelineLayout(glyphImagePipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule("GlyphImage.comp", computeShaderModuleDescriptor);

		ComputePipelineDescriptor glyphImageComputePipelineDescriptor{};
		glyphImageComputePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImageComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		glyphImageComputePipeline = Renderer::createComputePipeline(glyphImageComputePipelineDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImage() {
		// Contains every glyph distance fields.
		ImageDescriptor glyphMapImageDescriptor{};
		glyphMapImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapImageDescriptor.extent.width = GLYPH_MAP_IMAGE_EXTENT.width;
		glyphMapImageDescriptor.extent.height = GLYPH_MAP_IMAGE_EXTENT.height;
		glyphMapImageDescriptor.extent.depth = 1;
		glyphMapImageDescriptor.mipLevelCount = 1;
		glyphMapImageDescriptor.layerCount = 1;
		glyphMapImageDescriptor.sampleCount = 1;
		glyphMapImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::TRANSFER_SRC;
		glyphMapImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		glyphMapImage = Renderer::createImage(glyphMapImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImageView() {
		ImageViewDescriptor glyphMapImageViewDescriptor{};
		glyphMapImageViewDescriptor.mipLevelOffset = 0;
		glyphMapImageViewDescriptor.mipLevelCount = glyphMapImage->getMipLevelCount();
		glyphMapImageViewDescriptor.layerOffset = 0;
		glyphMapImageViewDescriptor.layerCount = glyphMapImage->getLayerCount();

		glyphMapImageView = glyphMapImage->createView(glyphMapImageViewDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfPipelineLayout() {
		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor{};
		glyphSdfPipelineLayoutDescriptor.descriptorSetLayouts = {glyphSdfDescriptorSetLayout};

		glyphSdfPipelineLayout = Renderer::createPipelineLayout(glyphSdfPipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule("GlyphDistanceField.comp", computeShaderModuleDescriptor);

		ComputePipelineDescriptor glyphSdfComputePipelineDescriptor{};
		glyphSdfComputePipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		glyphSdfComputePipeline = Renderer::createComputePipeline(glyphSdfComputePipelineDescriptor);
	}

	void GenerateSDFGlyphMapProcess::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = Renderer::getDevice()->allocateDescriptorSets({glyphImageDescriptorSetLayout, glyphSdfDescriptorSetLayout});

		glyphImageDescriptorSet = descriptorSets[0];
		glyphSdfDescriptorSet = descriptorSets[1];
	}

	void GenerateSDFGlyphMapProcess::updateDescriptorSets() const {
		DescriptorBuffer fontDescriptorBuffer{};
		fontDescriptorBuffer.buffer = fontBuffer;
		fontDescriptorBuffer.offset = 0;
		fontDescriptorBuffer.size = fontBuffer->size;

		DescriptorBuffer glyphDescriptorBuffer{};
		glyphDescriptorBuffer.buffer = glyphBuffer;
		glyphDescriptorBuffer.offset = 0;
		glyphDescriptorBuffer.size = glyphBuffer->size;

		DescriptorBuffer glyphOffsetDescriptorBuffer{};
		glyphOffsetDescriptorBuffer.buffer = glyphOffsetBuffer;
		glyphOffsetDescriptorBuffer.offset = 0;
		glyphOffsetDescriptorBuffer.size = glyphOffsetBuffer->size;

		DescriptorImage glyphDescriptorImage{};
		glyphDescriptorImage.imageView = glyphImageView;
		glyphDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		DescriptorImage glyphMapDescriptorImage{};
		glyphMapDescriptorImage.imageView = glyphMapImageView;
		glyphMapDescriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

		WriteDescriptorSet glyphImageFontWriteDescriptorSet{};
		glyphImageFontWriteDescriptorSet.descriptorSet = glyphImageDescriptorSet;
		glyphImageFontWriteDescriptorSet.bindingIndex = 0;
		glyphImageFontWriteDescriptorSet.descriptorBuffers = {fontDescriptorBuffer};

		WriteDescriptorSet glyphImageGlyphBufferWriteDescriptorSet{};
		glyphImageGlyphBufferWriteDescriptorSet.descriptorSet = glyphImageDescriptorSet;
		glyphImageGlyphBufferWriteDescriptorSet.bindingIndex = 1;
		glyphImageGlyphBufferWriteDescriptorSet.descriptorBuffers = {glyphDescriptorBuffer};

		WriteDescriptorSet glyphImageGlyphImageWriteDescriptorSet{};
		glyphImageGlyphImageWriteDescriptorSet.descriptorSet = glyphImageDescriptorSet;
		glyphImageGlyphImageWriteDescriptorSet.bindingIndex = 2;
		glyphImageGlyphImageWriteDescriptorSet.descriptorImages = {glyphDescriptorImage};

		WriteDescriptorSet glyphSdfGlyphImageWriteDescriptorSet{};
		glyphSdfGlyphImageWriteDescriptorSet.descriptorSet = glyphSdfDescriptorSet;
		glyphSdfGlyphImageWriteDescriptorSet.bindingIndex = 0;
		glyphSdfGlyphImageWriteDescriptorSet.descriptorImages = {glyphDescriptorImage};

		WriteDescriptorSet glyphSdfGlyphMapWriteDescriptorSet{};
		glyphSdfGlyphMapWriteDescriptorSet.descriptorSet = glyphSdfDescriptorSet;
		glyphSdfGlyphMapWriteDescriptorSet.bindingIndex = 1;
		glyphSdfGlyphMapWriteDescriptorSet.descriptorImages = {glyphMapDescriptorImage};

		WriteDescriptorSet glyphSdfFontWriteDescriptorSet{};
		glyphSdfFontWriteDescriptorSet.descriptorSet = glyphSdfDescriptorSet;
		glyphSdfFontWriteDescriptorSet.bindingIndex = 2;
		glyphSdfFontWriteDescriptorSet.descriptorBuffers = {fontDescriptorBuffer};

		WriteDescriptorSet glyphSdfGlyphOffsetWriteDescriptorSet{};
		glyphSdfGlyphOffsetWriteDescriptorSet.descriptorSet = glyphSdfDescriptorSet;
		glyphSdfGlyphOffsetWriteDescriptorSet.bindingIndex = 3;
		glyphSdfGlyphOffsetWriteDescriptorSet.descriptorBuffers = {glyphOffsetDescriptorBuffer};

		Renderer::getDevice()->updateDescriptorSets({
			glyphImageFontWriteDescriptorSet,
			glyphImageGlyphBufferWriteDescriptorSet,
			glyphImageGlyphImageWriteDescriptorSet,
			glyphSdfGlyphImageWriteDescriptorSet,
			glyphSdfGlyphMapWriteDescriptorSet,
			glyphSdfFontWriteDescriptorSet,
			glyphSdfGlyphOffsetWriteDescriptorSet,
		});
	}
}