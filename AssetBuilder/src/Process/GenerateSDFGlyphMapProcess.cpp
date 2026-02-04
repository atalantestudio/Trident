// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "GenerateSDFGlyphMapProcess.hpp"

namespace Trident {
	static bool calculateGlyphMapLayout(Extent2D glyphMapExtent, const File<FileFormat::TTF>& file, view<TrueTypeFont::Glyph> glyphView, uint32 spreadFactor) {
		const sint16 lineHeight = file.ascent - file.descent;
		const float32 inverseLineHeight = static_cast<float32>(1.0 / lineHeight * 64.0);
		const vec2<float32> spread = {spreadFactor * 2.0f, spreadFactor * 2.0f};

		uint32 x = 0;
		uint32 y = 0;
		uint32 rowHeight = 0;

		for (const TrueTypeFont::Glyph& glyph : glyphView) {
			if (glyph.emptyOrUnparsed()) {
				continue;
			}

			// The glyph size in TTF space.
			const vec2<float32> glyphFontSize = {static_cast<float32>(glyph.max.x - glyph.min.x), static_cast<float32>(glyph.max.y - glyph.min.y)};

			// The glyph size on the glyph map, in pixels.
			const vec2<float32> rawGlyphTexelSize = glyphFontSize * inverseLineHeight;

			// The glyph size on the glyph map, in pixels. Including the spread of the distance field.
			const vec2<float32> spreadGlyphTexelSize = rawGlyphTexelSize + spread;

			// TODO(lyah): Add vec<C, T> ceil(vec<C, T>, vec<C, T>).
			const uint32 glyphWidth = static_cast<uint32>(ceil(spreadGlyphTexelSize[0]));
			const uint32 glyphHeight = static_cast<uint32>(ceil(spreadGlyphTexelSize[1]));

			x += glyphWidth;
			rowHeight = max(rowHeight, glyphHeight);

			if (x >= glyphMapExtent[0]) {
				x = 0;
				y += rowHeight;
				rowHeight = 0;
			}

			if (y >= glyphMapExtent.height) {
				return false;
			}
		}

		return true;
	}

	static uint64 calculateGlyphBufferSize(const File<FileFormat::TTF>& file, view<TrueTypeFont::Glyph> glyphView) {
		uint64 maxGlyphEdgeCount = 0;

		for (const TrueTypeFont::Glyph& glyph : glyphView) {
			maxGlyphEdgeCount = max(maxGlyphEdgeCount, glyph.edges.count());
		}

		return sizeof(GenerateSDFGlyphMapProcess::GlyphBuffer) + maxGlyphEdgeCount * sizeof(GenerateSDFGlyphMapProcess::GlyphEdge);
	}

	GenerateSDFGlyphMapProcess::GenerateSDFGlyphMapProcess(const File<FileFormat::TTF>& file, SDFGlyphMap& output) {
		static constexpr uint32 GLYPH_SUBIMAGE_DIMENSION = 64;
		static constexpr uint32 spreadFactor = 7;

		const view<TrueTypeFont::Glyph> glyphView(&file.glyphs[0], file.glyphs.count());

		output.extent = {2048, 2048};
		output.glyphs = sequence<Glyph>(glyphView.count());

		if (!calculateGlyphMapLayout(output.extent, file, glyphView, spreadFactor)) {
			ENGINE_ASSERT(false, "The glyph map is too small.");

			return;
		}

		const uint64 glyphBufferSize = calculateGlyphBufferSize(file, glyphView);

		createFontBuffer();
		createGlyphBuffer(glyphBufferSize);
		createGlyphOffsetBuffer();

		createGlyphImage();
		createGlyphImageView();

		createGlyphMapImage(output.extent);
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

		ref<CommandEncoder> commandEncoder;

		const ref<Buffer> fontStagingBuffer = Renderer::createStagingBuffer(sizeof(FontStorage));
		const TrueTypeFont::FWord lineHeight = file.ascent - file.descent + file.lineGap;

		FontStorage* fontStagingBufferMappedRange = fontStagingBuffer->map<FontStorage>(0, fontStagingBuffer->getSize());
		fontStagingBufferMappedRange->spreadFactor = spreadFactor;
		fontStagingBufferMappedRange->outputGlyphSize = GLYPH_SUBIMAGE_DIMENSION;
		fontStagingBufferMappedRange->lineHeight = static_cast<sint32>(lineHeight);

		fontStagingBuffer->unmap();

		BufferCopyRegion fontBufferCopyRegion{};
		fontBufferCopyRegion.sourceOffset = 0;
		fontBufferCopyRegion.destinationOffset = 0;
		fontBufferCopyRegion.size = fontBuffer->getSize();

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		SET_DEBUG_NAME(commandBufferDescriptor, "Glyph map");

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontBuffer, {fontBufferCopyRegion});
		commandEncoder->setImageMemoryBarrier(glyphImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::GENERAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

		computeQueue->submit({commandEncoder->end()}, nullptr, nullptr, nullptr);
		computeQueue->waitIdle();

		FenceDescriptor writeBufferFenceDescriptor{};
		writeBufferFenceDescriptor.signaled = true;

		SET_DEBUG_NAME(writeBufferFenceDescriptor, "Buffer write");

		const ref<Fence> writeBufferFence = Renderer::createFence(writeBufferFenceDescriptor);
		const Extent3D& glyphMapExtent = glyphMapImage->getExtent();
		const float32 inverseLineHeight = static_cast<float32>(1.0 / lineHeight);
		const float32 scale = inverseLineHeight * static_cast<float32>(GLYPH_SUBIMAGE_DIMENSION);
		const float32 inverseGlyphMapWidth = static_cast<float32>(1.0 / glyphMapExtent.width);
		const float32 inverseGlyphMapHeight = static_cast<float32>(1.0 / glyphMapExtent.height);

		uint32 x = 0;
		uint32 y = 0;
		uint32 rowHeight = 0;
		uint16 glyphIndex = 0;
		uint16 edgeIndex;

		while (glyphIndex < glyphView.count()) {
			const TrueTypeFont::Glyph& glyph = glyphView[glyphIndex];
			const TrueTypeFont::longHorMetric& horizontalMetric = file.hMetrics[glyphIndex];

			Glyph& outputGlyph = output.glyphs[glyphIndex];
			outputGlyph.width = glyph.max.x - glyph.min.x;
			outputGlyph.advanceWidth = horizontalMetric.advanceWidth;
			outputGlyph.height = glyph.max.y - glyph.min.y;
			outputGlyph.leftSideBearing = horizontalMetric.leftSideBearing;
			outputGlyph.topSideBearing = file.ascent - glyph.max.y;
			outputGlyph.whitespace = glyph.emptyOrUnparsed();

			if (outputGlyph.whitespace) {
				glyphIndex += 1;

				continue;
			}

			// The glyph size in TTF units.
			const vec2<float32> ttfUnitSize = {static_cast<float32>(outputGlyph.width), static_cast<float32>(outputGlyph.height)};

			// The glyph size on the glyph map, in pixels.
			const vec2<float32> rawGlyphTexelSize = ttfUnitSize * scale;

			// The glyph size on the glyph map, in pixels. Including the spread of the distance field.
			const vec2<float32> spreadGlyphTexelSize = rawGlyphTexelSize + vec2<float32>(spreadFactor * 2.0f, spreadFactor * 2.0f);

			// TODO(lyah): Add vec<C, T> ceil(vec<C, T>, vec<C, T>).
			const uint32 glyphWidth = static_cast<uint32>(ceil(spreadGlyphTexelSize[0]));
			const uint32 glyphHeight = static_cast<uint32>(ceil(spreadGlyphTexelSize[1]));

			if (x + glyphWidth >= glyphMapExtent.width) {
				x = 0;
				y += rowHeight;
				rowHeight = 0;
			}

			glyphMappedRange->edgeCount = static_cast<uint32>(glyph.edges.count());

			GlyphEdge* glyphEdgeMappedRange = pointer_cast<GlyphEdge*>(glyphMappedRange + 1);

			edgeIndex = 0;

			while (edgeIndex < glyph.edges.count()) {
				const TrueTypeFont::Edge& edge = glyph.edges[edgeIndex];

				GlyphEdge& glyphEdge = glyphEdgeMappedRange[edgeIndex];
				glyphEdge.startX = edge.start.x - glyph.min.x;
				glyphEdge.startY = glyph.max.y - edge.start.y;
				glyphEdge.endX = edge.end.x - glyph.min.x;
				glyphEdge.endY = glyph.max.y - edge.end.y;
				glyphEdge.controlX = edge.control.x - glyph.min.x;
				glyphEdge.controlY = glyph.max.y - edge.control.y;
				glyphEdge.curved = edge.curved;

				edgeIndex += 1;
			}

			glyphOffsetMappedRange[0] = x;
			glyphOffsetMappedRange[1] = y;

			device->resetFence(writeBufferFence);

			commandEncoder->reset();
			commandEncoder->begin();
			commandEncoder->bindComputePipeline(glyphImageComputePipeline);
			commandEncoder->bindDescriptorSets(glyphImagePipelineLayout, {glyphImageDescriptorSet});

			// TODO: Dispatch less groups depending on the size of the area occupied by the glyph.
			commandEncoder->dispatch(128, 128, 1);

			commandEncoder->bindComputePipeline(glyphSdfComputePipeline);
			commandEncoder->bindDescriptorSets(glyphSdfPipelineLayout, {glyphSdfDescriptorSet});
			commandEncoder->dispatch(8, 8, 1);

			computeQueue->submit({commandEncoder->end()}, nullptr, nullptr, writeBufferFence);

			rowHeight = max(rowHeight, glyphHeight);

			outputGlyph.minCoordinate.x = static_cast<float32>(x + spreadFactor) * inverseGlyphMapWidth;
			outputGlyph.minCoordinate.y = static_cast<float32>(y + spreadFactor) * inverseGlyphMapHeight;
			outputGlyph.maxCoordinate.x = static_cast<float32>(x + spreadFactor + ceil(rawGlyphTexelSize[0])) * inverseGlyphMapWidth;
			outputGlyph.maxCoordinate.y = static_cast<float32>(y + spreadFactor + ceil(rawGlyphTexelSize[1])) * inverseGlyphMapHeight;

			x += glyphWidth;

			computeQueue->waitIdle();

			glyphIndex += 1;
		}

		BufferDescriptor glyphMapStagingBufferDescriptor{};
		glyphMapStagingBufferDescriptor.size = output.extent.width * output.extent.height;
		glyphMapStagingBufferDescriptor.usage = BufferUsage::TRANSFER_DST;
		glyphMapStagingBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;

		SET_DEBUG_NAME(glyphMapStagingBufferDescriptor, "Glyph map staging buffer");

		const ref<Buffer> glyphMapStagingBuffer = Renderer::createBuffer(glyphMapStagingBufferDescriptor);

		BufferImageCopyRegion glyphMapBufferImageCopyRegion{};
		glyphMapBufferImageCopyRegion.bufferOffset = 0;
		glyphMapBufferImageCopyRegion.imageOffsetX = 0;
		glyphMapBufferImageCopyRegion.imageOffsetY = 0;
		glyphMapBufferImageCopyRegion.imageOffsetZ = 0;
		glyphMapBufferImageCopyRegion.imageExtent = glyphMapImage->getExtent();
		glyphMapBufferImageCopyRegion.imageMipLevel = 0;
		glyphMapBufferImageCopyRegion.imageLayerOffset = 0;
		glyphMapBufferImageCopyRegion.imageLayerCount = glyphMapImage->getLayerCount();

		SET_DEBUG_NAME(commandBufferDescriptor, "Glyph map staging buffer copy");

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->setImageMemoryBarrier(glyphMapImage, ImageLayout::TRANSFER_SRC_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, {glyphMapBufferImageCopyRegion});

		computeQueue->submit({commandEncoder->end()}, nullptr, nullptr, nullptr);
		computeQueue->waitIdle();

		output.data = sequence<uint8>(glyphMapStagingBuffer->getSize());

		const uint8* glyphMapMappedRange = glyphMapStagingBuffer->map<uint8>(0, glyphMapStagingBuffer->getSize());

		std::copy_n(glyphMapMappedRange, glyphMapStagingBuffer->getSize(), &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	void GenerateSDFGlyphMapProcess::createFontBuffer() {
		BufferDescriptor fontBufferDescriptor{};
		fontBufferDescriptor.size = sizeof(FontStorage);
		fontBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		fontBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(fontBufferDescriptor, "Font");

		fontBuffer = Renderer::createBuffer(fontBufferDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphBuffer(uint64 size) {
		BufferDescriptor glyphBufferDescriptor{};
		glyphBufferDescriptor.size = size;
		glyphBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		glyphBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(glyphBufferDescriptor, "Glyph");

		glyphBuffer = Renderer::createBuffer(glyphBufferDescriptor);
		glyphMappedRange = glyphBuffer->map<GlyphBuffer>(0, glyphBuffer->getSize());
	}

	void GenerateSDFGlyphMapProcess::createGlyphOffsetBuffer() {
		BufferDescriptor glyphOffsetBufferDescriptor{};
		glyphOffsetBufferDescriptor.size = 3 * sizeof(float32);
		glyphOffsetBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		glyphOffsetBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphOffsetBufferDescriptor.persistent = true;

		SET_DEBUG_NAME(glyphOffsetBufferDescriptor, "Glyph offset");

		glyphOffsetBuffer = Renderer::createBuffer(glyphOffsetBufferDescriptor);
		glyphOffsetMappedRange = glyphOffsetBuffer->map<uint32>(0, glyphOffsetBuffer->getSize());
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

		SET_DEBUG_NAME(glyphImageDescriptor, "Glyph");

		glyphImage = Renderer::createImage(glyphImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageView() {
		ImageViewDescriptor glyphImageViewDescriptor{};
		glyphImageViewDescriptor.mipLevelOffset = 0;
		glyphImageViewDescriptor.mipLevelCount = 1;
		glyphImageViewDescriptor.layerOffset = 0;
		glyphImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(glyphImageViewDescriptor, "Glyph");

		glyphImageView = glyphImage->createView(glyphImageViewDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createDescriptorSetLayoutsAndSets() {
		DescriptorSetLayoutBinding fontBufferBinding{};
		fontBufferBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		fontBufferBinding.descriptorCount = 1;
		fontBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphBufferBinding{};
		glyphBufferBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
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
		glyphOffsetBufferBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		glyphOffsetBufferBinding.descriptorCount = 1;
		glyphOffsetBufferBinding.shaderStages = ShaderStage::COMPUTE;

		glyphImageDescriptorSetLayout = Renderer::createDescriptorSetLayout({fontBufferBinding, glyphBufferBinding, glyphImageBinding});
		glyphSdfDescriptorSetLayout = Renderer::createDescriptorSetLayout({glyphImageBinding, glyphMapImageBinding, fontBufferBinding, glyphOffsetBufferBinding});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImagePipelineLayout() {
		PipelineLayoutDescriptor glyphImagePipelineLayoutDescriptor{};
		glyphImagePipelineLayoutDescriptor.descriptorSetLayouts = {glyphImageDescriptorSetLayout};

		SET_DEBUG_NAME(glyphImagePipelineLayoutDescriptor, "Glyph");

		glyphImagePipelineLayout = Renderer::createPipelineLayout(glyphImagePipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(computeShaderModuleDescriptor, "Rasterize glyph");

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule(computeShaderModuleDescriptor, "RasterizeGlyph.comp");

		ComputePipelineDescriptor glyphImageComputePipelineDescriptor{};
		glyphImageComputePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImageComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		SET_DEBUG_NAME(glyphImageComputePipelineDescriptor, "Glyph image");

		glyphImageComputePipeline = Renderer::createComputePipeline(glyphImageComputePipelineDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImage(Extent2D extent) {
		// Contains every glyph distance fields.
		ImageDescriptor glyphMapImageDescriptor{};
		glyphMapImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapImageDescriptor.extent.width = extent.width;
		glyphMapImageDescriptor.extent.height = extent.height;
		glyphMapImageDescriptor.extent.depth = 1;
		glyphMapImageDescriptor.mipLevelCount = 1;
		glyphMapImageDescriptor.layerCount = 1;
		glyphMapImageDescriptor.sampleCount = 1;
		glyphMapImageDescriptor.usage = ImageUsage::STORAGE | ImageUsage::TRANSFER_SRC;
		glyphMapImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		SET_DEBUG_NAME(glyphMapImageDescriptor, "Glyph map");

		glyphMapImage = Renderer::createImage(glyphMapImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImageView() {
		ImageViewDescriptor glyphMapImageViewDescriptor{};
		glyphMapImageViewDescriptor.mipLevelOffset = 0;
		glyphMapImageViewDescriptor.mipLevelCount = 1;
		glyphMapImageViewDescriptor.layerOffset = 0;
		glyphMapImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(glyphMapImageViewDescriptor, "Glyph map");

		glyphMapImageView = glyphMapImage->createView(glyphMapImageViewDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfPipelineLayout() {
		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor{};
		glyphSdfPipelineLayoutDescriptor.descriptorSetLayouts = {glyphSdfDescriptorSetLayout};

		SET_DEBUG_NAME(glyphSdfPipelineLayoutDescriptor, "Glyph SDF");

		glyphSdfPipelineLayout = Renderer::createPipelineLayout(glyphSdfPipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(computeShaderModuleDescriptor, "Distance field");

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule(computeShaderModuleDescriptor, "DistanceField.comp");

		ComputePipelineDescriptor glyphSdfComputePipelineDescriptor{};
		glyphSdfComputePipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		SET_DEBUG_NAME(glyphSdfComputePipelineDescriptor, "Glyph SDF");

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
		fontDescriptorBuffer.offset = offsetof(FontStorage, lineHeight);
		fontDescriptorBuffer.size = sizeof(FontStorage::lineHeight);

		DescriptorBuffer fontDescriptorOffsetBuffer{};
		fontDescriptorOffsetBuffer.buffer = fontBuffer;
		fontDescriptorOffsetBuffer.offset = 0;
		fontDescriptorOffsetBuffer.size = sizeof(FontStorage::spreadFactor) + sizeof(FontStorage::outputGlyphSize);

		DescriptorBuffer glyphDescriptorBuffer{};
		glyphDescriptorBuffer.buffer = glyphBuffer;
		glyphDescriptorBuffer.offset = 0;
		glyphDescriptorBuffer.size = glyphBuffer->getSize();

		DescriptorBuffer glyphOffsetDescriptorBuffer{};
		glyphOffsetDescriptorBuffer.buffer = glyphOffsetBuffer;
		glyphOffsetDescriptorBuffer.offset = 0;
		glyphOffsetDescriptorBuffer.size = glyphOffsetBuffer->getSize();

		DescriptorImage glyphDescriptorImage{};
		glyphDescriptorImage.imageView = glyphImageView;
		glyphDescriptorImage.imageLayout = ImageLayout::GENERAL;

		DescriptorImage glyphMapDescriptorImage{};
		glyphMapDescriptorImage.imageView = glyphMapImageView;
		glyphMapDescriptorImage.imageLayout = ImageLayout::GENERAL;

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
		glyphSdfFontWriteDescriptorSet.descriptorBuffers = {fontDescriptorOffsetBuffer};

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