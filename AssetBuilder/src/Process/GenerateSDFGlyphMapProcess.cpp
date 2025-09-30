#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "GenerateSDFGlyphMapProcess.hpp"

namespace ProjectA {
	static constexpr uint64 MAX_GLYPH_COUNT = 2048;
	static constexpr Extent2D GLYPH_MAP_IMAGE_EXTENT = {2048, 2048};
	static constexpr uint32 GLYPH_SUBIMAGE_DIMENSION = 64;
	static constexpr uint32 SPREAD_FACTOR = 0;

	GenerateSDFGlyphMapProcess::GenerateSDFGlyphMapProcess(const File<FileFormat::TTF>& file, SDFGlyphMap& output) {
		ENGINE_ASSERT(file.glyphs.count < MAX_GLYPH_COUNT, "The input font must have less than [] glyphs.", MAX_GLYPH_COUNT);

		output.extent = GLYPH_MAP_IMAGE_EXTENT;
		output.glyphs = sequence<Glyph>(file.glyphs.count);

		const ref<Device> device = Renderer::getDevice();
		const ref<Queue> computeQueue = device->getQueue(QueueFamilyCapability::COMPUTE);

		CommandBufferDescriptor autoCommandBufferDescriptor{};
		autoCommandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		CommandBufferDescriptor oneTimeSubmitCommandBufferDescriptor{};
		oneTimeSubmitCommandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		ref<CommandEncoder> commandEncoder;
		ref<CommandBuffer> commandBuffer;

		const uint64 glyphBufferSize = computeMaxGlyphByteSize(file.glyphs);

		constexpr uint64 fontBufferSize = sizeof(FontStorage);

		const ref<Buffer> fontStagingBuffer = Renderer::createStagingBuffer(fontBufferSize);

		FontStorage* fontMappedRange = fontStagingBuffer->map<FontStorage>(0, fontBufferSize);

		fontMappedRange->outputGlyphSize = GLYPH_SUBIMAGE_DIMENSION;
		fontMappedRange->spreadFactor = SPREAD_FACTOR;
		fontMappedRange->ascent = file.ascent;
		fontMappedRange->descent = file.descent;

		fontStagingBuffer->unmap();

		BufferDescriptor fontStorageBufferDescriptor;
		fontStorageBufferDescriptor.size = fontBufferSize;
		fontStorageBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		fontStorageBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		const ref<Buffer> fontStorageBuffer = Renderer::createBuffer(fontStorageBufferDescriptor);

		BufferDescriptor glyphBufferDescriptor;
		glyphBufferDescriptor.size = glyphBufferSize;
		glyphBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		glyphBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphBufferDescriptor.persistent = true;

		const ref<Buffer> glyphBuffer = Renderer::createBuffer(glyphBufferDescriptor);

		GlyphBuffer* glyphMappedRange = glyphBuffer->map<GlyphBuffer>(0, glyphBufferSize);

		constexpr uint64 glyphOffsetBufferSize = 3 * sizeof(float32);

		BufferDescriptor glyphOffsetBufferDescriptor;
		glyphOffsetBufferDescriptor.size = glyphOffsetBufferSize;
		glyphOffsetBufferDescriptor.usage = BufferUsage::STORAGE | BufferUsage::TRANSFER_DST;
		glyphOffsetBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphOffsetBufferDescriptor.persistent = true;

		const ref<Buffer> glyphOffsetBuffer = Renderer::createBuffer(glyphOffsetBufferDescriptor);

		uint32* glyphOffsetMappedRange = glyphOffsetBuffer->map<uint32>(0, glyphOffsetBufferSize);

		// Used for generating the distance field for each glyph.
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

		const ref<Image> glyphImage = Renderer::createImage(glyphImageDescriptor);

		ImageViewDescriptor glyphImageViewDescriptor;
		glyphImageViewDescriptor.mipLevelOffset = 0;
		glyphImageViewDescriptor.mipLevelCount = glyphImage->getMipLevelCount();
		glyphImageViewDescriptor.layerOffset = 0;
		glyphImageViewDescriptor.layerCount = glyphImage->getLayerCount();

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

		ComputePipelineDescriptor glyphImagePipelineDescriptor;
		glyphImagePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImagePipelineDescriptor.computeShaderModule = Renderer::loadShaderModule("GlyphImage.comp", glyphImageShaderModuleDescriptor);

		const ref<ComputePipeline> glyphImagePipeline = Renderer::createComputePipeline(glyphImagePipelineDescriptor);

		// Contains every glyph distance fields.
		ImageDescriptor glyphMapSdfImageDescriptor{};
		glyphMapSdfImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapSdfImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapSdfImageDescriptor.extent.width = output.extent.width;
		glyphMapSdfImageDescriptor.extent.height = output.extent.height;
		glyphMapSdfImageDescriptor.extent.depth = 1;
		glyphMapSdfImageDescriptor.mipLevelCount = 1;
		glyphMapSdfImageDescriptor.layerCount = 1;
		glyphMapSdfImageDescriptor.sampleCount = 1;
		glyphMapSdfImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::TRANSFER_SRC;
		glyphMapSdfImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		ref<Image> glyphMapImage = Renderer::createImage(glyphMapSdfImageDescriptor);

		ImageViewDescriptor glyphMapSdfImageViewDescriptor{};
		glyphMapSdfImageViewDescriptor.mipLevelOffset = 0;
		glyphMapSdfImageViewDescriptor.mipLevelCount = glyphMapImage->getMipLevelCount();
		glyphMapSdfImageViewDescriptor.layerOffset = 0;
		glyphMapSdfImageViewDescriptor.layerCount = glyphMapImage->getLayerCount();

		const ref<ImageView> glyphMapSdfImageView = glyphMapImage->createView(glyphMapSdfImageViewDescriptor);

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
			fontBufferDescriptorSetLayoutBinding,
			glyphIndexDescriptorSetLayoutBinding,
		};

		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor;
		glyphSdfPipelineLayoutDescriptor.layouts = {glyphSdfDescriptorSetLayout};

		const ref<PipelineLayout> glyphSdfPipelineLayout = Renderer::createPipelineLayout(glyphSdfPipelineLayoutDescriptor);

		ShaderModuleDescriptor glyphSdfShaderModuleDescriptor{};
		glyphSdfShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		glyphSdfShaderModuleDescriptor.entryPoint = "main";

		ComputePipelineDescriptor glyphSdfPipelineDescriptor;
		glyphSdfPipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfPipelineDescriptor.computeShaderModule = Renderer::loadShaderModule("GlyphDistanceField.comp", glyphSdfShaderModuleDescriptor);

		const ref<ComputePipeline> glyphSdfPipeline = Renderer::createComputePipeline(glyphSdfPipelineDescriptor);

		// Copy the font staging buffer to the font storage buffer and transition the glyph and glyph map images from UNDEFINED to GENERAL.

		sequence<BufferCopyRegion> fontBufferCopyRegions(1);

		BufferCopyRegion& fontBufferCopyRegion = fontBufferCopyRegions[0];
		fontBufferCopyRegion.sourceOffset = 0;
		fontBufferCopyRegion.destinationOffset = 0;
		fontBufferCopyRegion.size = fontBufferSize;

		commandEncoder = computeQueue->createCommandEncoder(oneTimeSubmitCommandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontStorageBuffer, fontBufferCopyRegions);
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		commandBuffer = commandEncoder->end();

		computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();

		float32 xOffset = 0;
		float32 yOffset = 0;

		const Extent3D& glyphMapExtent = glyphMapImage->getExtent();
		const float32 lineHeight = static_cast<float32>(file.ascent - file.descent);

		commandEncoder = computeQueue->createCommandEncoder(autoCommandBufferDescriptor);

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
			commandEncoder->bindComputePipeline(glyphImagePipeline);
			commandEncoder->bindAllDescriptorSets();

			// TODO: Dispatch less groups depending on the size of the glyph area.
			commandEncoder->dispatch(128, 128, 1);

			commandEncoder->bindComputePipeline(glyphSdfPipeline);
			commandEncoder->bindAllDescriptorSets();

			// TODO: Support custom glyph sub-image dimension (multiply down). See GlyphDistanceField.comp.
			commandEncoder->dispatch(8, 8, 1);

			commandBuffer = commandEncoder->end();

			computeQueue->submit({commandBuffer}, nullptr, nullptr, writeBufferFence);

			device->waitIdle();

			xOffset += ceil(width) + SPREAD_FACTOR * 2;
		}

		// Transition the glyph map image to TRANSFER_SRC_OPTIMAL and copy its data to the final buffer.

		BufferDescriptor glyphMapStagingBufferDescriptor;
		glyphMapStagingBufferDescriptor.size = output.extent.width * output.extent.height;
		glyphMapStagingBufferDescriptor.usage = BufferUsage::TRANSFER_DST;
		glyphMapStagingBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphMapStagingBufferDescriptor.persistent = false;

		ref<Buffer> glyphMapStagingBuffer = Renderer::createBuffer(glyphMapStagingBufferDescriptor);

		sequence<BufferImageCopyRegion> bufferImageCopyRegions(1);
		BufferImageCopyRegion& glyphMapCopyRegion = bufferImageCopyRegions[0];
		glyphMapCopyRegion.bufferOffset = 0;
		glyphMapCopyRegion.imageOffsetX = 0;
		glyphMapCopyRegion.imageOffsetY = 0;
		glyphMapCopyRegion.imageOffsetZ = 0;
		glyphMapCopyRegion.imageExtent = glyphMapImage->getExtent();
		glyphMapCopyRegion.imageMipLevel = 0;
		glyphMapCopyRegion.imageLayerOffset = 0;
		glyphMapCopyRegion.imageLayerCount = glyphMapImage->getLayerCount();

		commandEncoder = computeQueue->createCommandEncoder(oneTimeSubmitCommandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::TRANSFER_SRC_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, bufferImageCopyRegions);

		commandBuffer = commandEncoder->end();

		computeQueue->submit({commandBuffer}, nullptr, nullptr, nullptr);

		device->waitIdle();

		output.data = sequence<uint8>(glyphMapStagingBuffer->size);

		const uint8* glyphMapStagingBufferMappedRange = glyphMapStagingBuffer->map<uint8>(0, glyphMapStagingBuffer->size);

		std::copy(glyphMapStagingBufferMappedRange, glyphMapStagingBufferMappedRange + glyphMapStagingBuffer->size, &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	uint64 GenerateSDFGlyphMapProcess::computeMaxGlyphByteSize(const sequence<TrueTypeFont::Glyph>& glyphs) {
		uint64 maxGlyphEdgeCount = 0;

		for (const TrueTypeFont::Glyph& glyph : glyphs) {
			maxGlyphEdgeCount = max(maxGlyphEdgeCount, glyph.edges.count);
		}

		return sizeof(GlyphBuffer) + maxGlyphEdgeCount * sizeof(GlyphEdge);
	}
}