// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "../File/TTF.hpp"
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
		// The remaining glyphs will be ignored.
		static constexpr uint64 MAX_GLYPH_COUNT = 737;

		static constexpr uint32 GLYPH_SUBIMAGE_DIMENSION = 64;
		static constexpr uint32 spreadFactor = 7;

		const view<TrueTypeFont::Glyph> glyphView(&file.glyphs[0], min(MAX_GLYPH_COUNT, file.glyphs.count()));

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

		const ref<Buffer<FontStorage>> fontStagingBuffer = Renderer::createStagingBuffer<FontStorage>(sizeof(FontStorage));
		const TrueTypeFont::FWord lineHeight = file.ascent - file.descent + file.lineGap;

		FontStorage* fontStagingBufferMappedRange = fontStagingBuffer->map(0, fontStagingBuffer->getSize());
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

		#if defined(DEBUG_OBJECT_NAME)
			commandBufferDescriptor.name = "Glyph map";
		#endif

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontBuffer, {fontBufferCopyRegion});

		const sequence<ImageMemoryBarrier> imageMemoryBarriers = {
			// Glyph
			{
				.image = glyphImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_READ | Access::SHADER_WRITE,
			},

			// Glyph map
			{
				.image = glyphMapImage,
				.newLayout = ImageLayout::GENERAL,
				.newAccess = Access::SHADER_WRITE,
			},
		};

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		ref<CommandBuffer> commandBuffer = commandEncoder->end();

		computeQueue->submit(commandBuffer);
		computeQueue->waitIdle();

		const FenceDescriptor writeBufferFenceDescriptor = {
			.signaled = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Buffer write",
			#endif
		};

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
			const vec2<float32> spreadGlyphTexelSize = rawGlyphTexelSize + vec2<float32>{spreadFactor * 2.0f, spreadFactor * 2.0f};

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
			commandEncoder->bindDescriptorSets(glyphImagePipelineLayout, 0, glyphImageDescriptorSet);

			// TODO: Dispatch less groups depending on the size of the area occupied by the glyph.
			commandEncoder->dispatch(128, 128, 1);

			commandEncoder->bindComputePipeline(glyphSdfComputePipeline);
			commandEncoder->bindDescriptorSets(glyphSdfPipelineLayout, 0, glyphSdfDescriptorSet);
			commandEncoder->dispatch(8, 8, 1);

			commandBuffer = commandEncoder->end();

			computeQueue->submit(commandBuffer, nullptr, nullptr, writeBufferFence);

			rowHeight = max(rowHeight, glyphHeight);

			outputGlyph.minCoordinate.x = (x + spreadFactor) * inverseGlyphMapWidth;
			outputGlyph.minCoordinate.y = (y + spreadFactor) * inverseGlyphMapHeight;
			outputGlyph.maxCoordinate.x = (x + spreadFactor + rawGlyphTexelSize[0] + 1) * inverseGlyphMapWidth;
			outputGlyph.maxCoordinate.y = (y + spreadFactor + rawGlyphTexelSize[1] + 1) * inverseGlyphMapHeight;

			x += glyphWidth;

			computeQueue->waitIdle();

			glyphIndex += 1;
		}

		const ref<Buffer<uint8>> glyphMapStagingBuffer = Renderer::createBuffer<uint8>({
			.size = output.extent.width * output.extent.height,
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map staging",
			#endif
		});

		const BufferImageCopyRegion glyphMapBufferImageCopyRegion = {
			.bufferOffset = 0,
			.imageOffsetX = 0,
			.imageOffsetY = 0,
			.imageOffsetZ = 0,
			.imageExtent = glyphMapImage->getExtent(),
			.imageMipLevel = 0,
			.imageLayerOffset = 0,
			.imageLayerCount = glyphMapImage->getLayerCount(),
		};

		#if defined(BUILD_MODE_DEBUG)
			commandBufferDescriptor.name = "Copy glyph map image";
		#endif

		commandEncoder = computeQueue->createCommandEncoder(commandBufferDescriptor);
		commandEncoder->begin();

		const ImageMemoryBarrier glyphMapImageMemoryBarrier = {
			.image = glyphMapImage,
			.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
			.newAccess = Access::TRANSFER_READ,
		};

		commandEncoder->insertMemoryBarriers({}, glyphMapImageMemoryBarrier);
		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, glyphMapBufferImageCopyRegion);

		commandBuffer = commandEncoder->end();

		computeQueue->submit(commandBuffer);
		computeQueue->waitIdle();

		output.data = sequence<uint8>(glyphMapStagingBuffer->getSize());

		const uint8* glyphMapMappedRange = glyphMapStagingBuffer->map(0, glyphMapStagingBuffer->getSize());

		std::copy_n(glyphMapMappedRange, glyphMapStagingBuffer->getSize(), &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	void GenerateSDFGlyphMapProcess::createFontBuffer() {
		BufferDescriptor fontBufferDescriptor{};
		fontBufferDescriptor.size = sizeof(FontStorage);
		fontBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		fontBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		#if defined(DEBUG_OBJECT_NAME)
			fontBufferDescriptor.name = "Font";
		#endif

		fontBuffer = Renderer::createBuffer<FontStorage>(fontBufferDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphBuffer(uint64 size) {
		BufferDescriptor glyphBufferDescriptor{};
		glyphBufferDescriptor.size = size;
		glyphBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		glyphBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphBufferDescriptor.persistent = true;

		#if defined(DEBUG_OBJECT_NAME)
			glyphBufferDescriptor.name = "Glyph";
		#endif

		glyphBuffer = Renderer::createBuffer<GlyphBuffer>(glyphBufferDescriptor);
		glyphMappedRange = glyphBuffer->map(0, glyphBuffer->getSize());
	}

	void GenerateSDFGlyphMapProcess::createGlyphOffsetBuffer() {
		BufferDescriptor glyphOffsetBufferDescriptor{};
		glyphOffsetBufferDescriptor.size = 3 * sizeof(float32);
		glyphOffsetBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
		glyphOffsetBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		glyphOffsetBufferDescriptor.persistent = true;

		#if defined(DEBUG_OBJECT_NAME)
			glyphOffsetBufferDescriptor.name = "Glyph offset";
		#endif

		glyphOffsetBuffer = Renderer::createBuffer<uint32>(glyphOffsetBufferDescriptor);
		glyphOffsetMappedRange = glyphOffsetBuffer->map(0, glyphOffsetBuffer->getSize());
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

		#if defined(DEBUG_OBJECT_NAME)
			glyphImageDescriptor.name = "Glyph";
		#endif

		glyphImage = Renderer::createImage(glyphImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageView() {
		ImageViewDescriptor glyphImageViewDescriptor{};
		glyphImageViewDescriptor.mipLevelOffset = 0;
		glyphImageViewDescriptor.mipLevelCount = 1;
		glyphImageViewDescriptor.layerOffset = 0;
		glyphImageViewDescriptor.layerCount = 1;

		#if defined(DEBUG_OBJECT_NAME)
			glyphImageViewDescriptor.name = "Glyph";
		#endif

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

		DescriptorSetLayoutBinding readWriteGlyphImageBinding{};
		readWriteGlyphImageBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE;
		readWriteGlyphImageBinding.descriptorCount = 1;
		readWriteGlyphImageBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding readOnlyGlyphImageBinding{};
		readOnlyGlyphImageBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE;
		readOnlyGlyphImageBinding.descriptorCount = 1;
		readOnlyGlyphImageBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphMapImageBinding{};
		glyphMapImageBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE;
		glyphMapImageBinding.descriptorCount = 1;
		glyphMapImageBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding glyphOffsetBufferBinding{};
		glyphOffsetBufferBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		glyphOffsetBufferBinding.descriptorCount = 1;
		glyphOffsetBufferBinding.shaderStages = ShaderStage::COMPUTE;

		glyphImageDescriptorSetLayout = Renderer::createDescriptorSetLayout({fontBufferBinding, glyphBufferBinding, readWriteGlyphImageBinding});
		glyphSdfDescriptorSetLayout = Renderer::createDescriptorSetLayout({readOnlyGlyphImageBinding, glyphMapImageBinding, fontBufferBinding, glyphOffsetBufferBinding});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImagePipelineLayout() {
		PipelineLayoutDescriptor glyphImagePipelineLayoutDescriptor{};
		glyphImagePipelineLayoutDescriptor.descriptorSetLayouts = {glyphImageDescriptorSetLayout};

		#if defined(DEBUG_OBJECT_NAME)
			glyphImagePipelineLayoutDescriptor.name = "Glyph";
		#endif

		glyphImagePipelineLayout = Renderer::createPipelineLayout(glyphImagePipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			computeShaderModuleDescriptor.name = "Rasterize glyph";
		#endif

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule(computeShaderModuleDescriptor, "RasterizeGlyph.comp");

		ComputePipelineDescriptor glyphImageComputePipelineDescriptor{};
		glyphImageComputePipelineDescriptor.pipelineLayout = glyphImagePipelineLayout;
		glyphImageComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		#if defined(DEBUG_OBJECT_NAME)
			glyphImageComputePipelineDescriptor.name = "Glyph image";
		#endif

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

		#if defined(DEBUG_OBJECT_NAME)
			glyphMapImageDescriptor.name = "Glyph map";
		#endif

		glyphMapImage = Renderer::createImage(glyphMapImageDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImageView() {
		ImageViewDescriptor glyphMapImageViewDescriptor{};
		glyphMapImageViewDescriptor.mipLevelOffset = 0;
		glyphMapImageViewDescriptor.mipLevelCount = 1;
		glyphMapImageViewDescriptor.layerOffset = 0;
		glyphMapImageViewDescriptor.layerCount = 1;

		#if defined(DEBUG_OBJECT_NAME)
			glyphMapImageViewDescriptor.name = "Glyph map";
		#endif

		glyphMapImageView = glyphMapImage->createView(glyphMapImageViewDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfPipelineLayout() {
		PipelineLayoutDescriptor glyphSdfPipelineLayoutDescriptor{};
		glyphSdfPipelineLayoutDescriptor.descriptorSetLayouts = {glyphSdfDescriptorSetLayout};

		#if defined(DEBUG_OBJECT_NAME)
			glyphSdfPipelineLayoutDescriptor.name = "Glyph SDF";
		#endif

		glyphSdfPipelineLayout = Renderer::createPipelineLayout(glyphSdfPipelineLayoutDescriptor);
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfComputePipeline() {
		ShaderModuleDescriptor computeShaderModuleDescriptor{};
		computeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		computeShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			computeShaderModuleDescriptor.name = "Distance field";
		#endif

		const ref<ShaderModule> computeShaderModule = Renderer::loadShaderModule(computeShaderModuleDescriptor, "DistanceField.comp");

		ComputePipelineDescriptor glyphSdfComputePipelineDescriptor{};
		glyphSdfComputePipelineDescriptor.pipelineLayout = glyphSdfPipelineLayout;
		glyphSdfComputePipelineDescriptor.computeShaderModule = computeShaderModule;

		#if defined(DEBUG_OBJECT_NAME)
			glyphSdfComputePipelineDescriptor.name = "Glyph SDF";
		#endif

		glyphSdfComputePipeline = Renderer::createComputePipeline(glyphSdfComputePipelineDescriptor);
	}

	void GenerateSDFGlyphMapProcess::allocateDescriptorSets() {
		const sequence<ref<DescriptorSet>> descriptorSets = Renderer::getDevice()->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
			glyphImageDescriptorSetLayout,
			glyphSdfDescriptorSetLayout,
		});

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
		glyphDescriptorImage.sampler = nullptr;

		DescriptorImage glyphMapDescriptorImage{};
		glyphMapDescriptorImage.imageView = glyphMapImageView;
		glyphMapDescriptorImage.imageLayout = ImageLayout::GENERAL;
		glyphMapDescriptorImage.sampler = nullptr;

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 0,
				.descriptorBuffers = {fontDescriptorBuffer},
			},
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 1,
				.descriptorBuffers = {glyphDescriptorBuffer},
			},
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 2,
				.descriptorImages = {glyphDescriptorImage},
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = {glyphDescriptorImage},
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = {glyphMapDescriptorImage},
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 2,
				.descriptorBuffers = {fontDescriptorOffsetBuffer},
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 3,
				.descriptorBuffers = {glyphOffsetDescriptorBuffer},
			},
		};

		// TODO: Obtain a device from LayerCompositor/GraphicsAdapter.
		Renderer::getDevice()->updateDescriptorSets(writeDescriptorSets);
	}
}