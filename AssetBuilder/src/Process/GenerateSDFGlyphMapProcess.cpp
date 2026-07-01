// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "../File/TTF.hpp"
#include "GenerateSDFGlyphMapProcess.hpp"

namespace Trident {
	static uint64 calculateGlyphMapLayout(Extent2D glyphMapExtent, const File<FileFormat::TTF>& file, view<TrueTypeFont::Glyph> glyphView, uint32 spreadFactor) {
		const sint16 lineHeight = file.ascent - file.descent;
		const float32 inverseLineHeight = static_cast<float32>(1.0 / lineHeight * 64.0);
		const vec2<float32> spread = {spreadFactor * 2.0f, spreadFactor * 2.0f};

		uint32 x = 0;
		uint32 y = 0;
		uint32 rowHeight = 0;
		uint64 glyphCount = 0;

		while (glyphCount < glyphView.count()) {
			const TrueTypeFont::Glyph& glyph = glyphView[glyphCount];

			if (glyph.emptyOrUnparsed()) {
				glyphCount += 1;

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
				break;
			}

			glyphCount += 1;
		}

		return glyphCount;
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

		output.extent = {2048, 2048};

		view<TrueTypeFont::Glyph> glyphs(&file.glyphs[0], file.glyphs.count());

		const uint64 glyphCount = calculateGlyphMapLayout(output.extent, file, glyphs, spreadFactor);

		if (glyphCount < glyphs.count()) {
			ENGINE_WARNING("The total size needed for the glyphs exceeds the size of the glyph map. [] glyphs were ignored.", glyphs.count() - glyphCount);

			glyphs = view<TrueTypeFont::Glyph>(&file.glyphs[0], glyphCount);
		}

		output.glyphs = sequence<Glyph>(glyphs.count());

		const uint64 glyphBufferSize = calculateGlyphBufferSize(file, glyphs);
		const TrueTypeFont::FWord lineHeight = file.ascent - file.descent + file.lineGap;

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

		const ref<Buffer<FontStorage>> fontStagingBuffer = Renderer::createStagingBuffer<FontStorage>(fontBuffer->getSize());
		const ref<BufferView<FontStorage>> fontStagingView = fontStagingBuffer->createView<FontStorage>(0, fontStagingBuffer->getSize());

		FontStorage* fontStagingBufferMappedRange = fontStagingView->map();
		fontStagingBufferMappedRange->spreadFactor = spreadFactor;
		fontStagingBufferMappedRange->outputGlyphSize = GLYPH_SUBIMAGE_DIMENSION;
		fontStagingBufferMappedRange->lineHeight = static_cast<sint32>(lineHeight);

		fontStagingBuffer->unmap();

		const BufferCopyRegion fontBufferCopyRegion = {
			.sourceOffset = 0,
			.destinationOffset = 0,
			.size = fontBuffer->getSize(),
		};

		const ref<Device> device = Renderer::getDevice();
		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::COMPUTE);

		ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Generate glyph map",
			#endif
		});
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontBuffer, fontBufferCopyRegion);

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

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<uint8>> glyphView = glyphBuffer->createView<uint8>(0, glyphBuffer->getSize());
		const ref<BufferView<uint32>> glyphOffsetView = glyphOffsetBuffer->createView<uint32>(0, glyphOffsetBuffer->getSize());

		GlyphBuffer* glyphMappedRange = pointer_cast<GlyphBuffer*>(glyphView->map());
		uint32* glyphOffsetMappedRange = glyphOffsetView->map();

		const ref<Fence> writeBufferFence = Renderer::createFence( {
			.signaled = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Buffer write",
			#endif
		});
		const Extent3D& glyphMapExtent = glyphMapImage->getExtent();
		const float32 inverseLineHeight = static_cast<float32>(1.0 / lineHeight);
		const float32 scale = inverseLineHeight * static_cast<float32>(GLYPH_SUBIMAGE_DIMENSION);
		const float32 inverseGlyphMapWidth = static_cast<float32>(1.0 / glyphMapExtent.width);
		const float32 inverseGlyphMapHeight = static_cast<float32>(1.0 / glyphMapExtent.height);

		uint32 x = 0;
		uint32 y = 0;
		uint32 rowHeight = 0;
		uint64 glyphIndex = 0;
		uint16 edgeIndex;

		while (glyphIndex < glyphs.count()) {
			const TrueTypeFont::Glyph& glyph = glyphs[glyphIndex];

			// TODO(asset-builder/font/ttf/metric-overflow)
			// On some fonts (e.g. Bodoni Black) there is one less hMetric entry.
			// For now, overflowing glyphs use the last entry. This seems coherent with FontDrop!.
			const TrueTypeFont::longHorMetric& horizontalMetric = file.hMetrics[min(glyphIndex, file.hMetrics.count() - 1)];

			Glyph& outputGlyph = output.glyphs[glyphIndex];
			outputGlyph.width = glyph.max.x - glyph.min.x;
			outputGlyph.advanceWidth = horizontalMetric.advanceWidth;
			outputGlyph.height = glyph.max.y - glyph.min.y;
			outputGlyph.leftSideBearing = horizontalMetric.leftSideBearing;
			outputGlyph.yMax = glyph.max.y;
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

			queue->submit(commandBuffer, nullptr, nullptr, writeBufferFence);
			queue->waitIdle();

			outputGlyph.minCoordinate.x = (x + spreadFactor) * inverseGlyphMapWidth;
			outputGlyph.minCoordinate.y = (y + spreadFactor) * inverseGlyphMapHeight;
			outputGlyph.maxCoordinate.x = (x + spreadFactor + rawGlyphTexelSize[0] + 1) * inverseGlyphMapWidth;
			outputGlyph.maxCoordinate.y = (y + spreadFactor + rawGlyphTexelSize[1] + 1) * inverseGlyphMapHeight;

			x += glyphWidth;
			rowHeight = max(rowHeight, glyphHeight);
			glyphIndex += 1;
		}

		glyphBuffer->unmap();
		glyphOffsetBuffer->unmap();

		const ref<BaseBuffer> glyphMapStagingBuffer = Renderer::createDynamicBuffer({
			.size = output.extent.width * output.extent.height,
			.usage = BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map staging",
			#endif
		});

		commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Copy glyph map image",
			#endif
		});
		commandEncoder->begin();

		const ImageMemoryBarrier glyphMapImageMemoryBarrier = {
			.image = glyphMapImage,
			.newLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
			.newAccess = Access::TRANSFER_READ,
		};

		commandEncoder->insertMemoryBarriers({}, glyphMapImageMemoryBarrier);

		const BufferImageCopyRegion glyphMapBufferImageCopyRegion = {
			.bufferOffset = 0,
			.imageOffsetX = 0,
			.imageOffsetY = 0,
			.imageOffsetZ = 0,
			.imageExtent = glyphMapImage->getExtent(),
			.imageMipLevel = glyphMapImageView->getMipLevelOffset(),
			.imageLayerOffset = glyphMapImageView->getLayerOffset(),
			.imageLayerCount = glyphMapImageView->getLayerCount(),
		};

		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, glyphMapBufferImageCopyRegion);

		commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const ref<BufferView<uint8>> glyphMapView = glyphMapStagingBuffer->createView<uint8>(0, glyphMapStagingBuffer->getSize());
		const uint8* glyphMapMappedRange = glyphMapView->map();

		output.data = sequence<uint8>(output.extent.width * output.extent.height);

		std::copy_n(glyphMapMappedRange, glyphMapStagingBuffer->getSize(), &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	void GenerateSDFGlyphMapProcess::createFontBuffer() {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		fontBuffer = Renderer::createDynamicBuffer({
			.size = ceil(sizeof(FontStorage), minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Font",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphBuffer(uint64 size) {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		glyphBuffer = Renderer::createDynamicBuffer({
			.size = ceil(size, minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphOffsetBuffer() {
		const uint64 minStorageBufferOffsetAlignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		glyphOffsetBuffer = Renderer::createDynamicBuffer({
			.size = ceil(3 * sizeof(float32), minStorageBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph offset",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImage() {
		glyphImage = Renderer::createDynamicImage({
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8_UNORM,
			.extent = {4096, 4096, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::STORAGE | ImageUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageView() {
		glyphImageView = glyphImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createDescriptorSetLayoutsAndSets() {
		const DescriptorSetLayoutBinding fontBufferBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};
		const DescriptorSetLayoutBinding glyphBufferBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};
		const DescriptorSetLayoutBinding readWriteGlyphImageBinding = {
			.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};
		const DescriptorSetLayoutBinding readOnlyGlyphImageBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};
		const DescriptorSetLayoutBinding glyphMapImageBinding = {
			.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};
		const DescriptorSetLayoutBinding glyphOffsetBufferBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};

		glyphImageDescriptorSetLayout = Renderer::createDescriptorSetLayout({fontBufferBinding, glyphBufferBinding, readWriteGlyphImageBinding});
		glyphSdfDescriptorSetLayout = Renderer::createDescriptorSetLayout({readOnlyGlyphImageBinding, glyphMapImageBinding, fontBufferBinding, glyphOffsetBufferBinding});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImagePipelineLayout() {
		glyphImagePipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {glyphImageDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph image",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphImageComputePipeline() {
		glyphImageComputePipeline = Renderer::createComputePipeline({
			.pipelineLayout = glyphImagePipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rasterize glyph",
					#endif
				},
				"RasterizeGlyph.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph image",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImage(Extent2D extent) {
		glyphMapImage = Renderer::createDynamicImage({
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8_UNORM,
			.extent = {extent.width, extent.height, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::STORAGE | ImageUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphMapImageView() {
		glyphMapImageView = glyphMapImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfPipelineLayout() {
		glyphSdfPipelineLayout = Renderer::createPipelineLayout({
			.descriptorSetLayouts = {glyphSdfDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph SDF",
			#endif
		});
	}

	void GenerateSDFGlyphMapProcess::createGlyphSdfComputePipeline() {
		glyphSdfComputePipeline = Renderer::createComputePipeline({
			.pipelineLayout = glyphSdfPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Distance field",
					#endif
				},
				"DistanceField.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph SDF",
			#endif
		});
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
		const ref<BufferView<FontStorage>> fontView = fontBuffer->createView<FontStorage>(0, fontBuffer->getSize());
		const ref<BufferView<uint8>> glyphView = glyphBuffer->createView<uint8>(0, glyphBuffer->getSize());
		const ref<BufferView<uint32>> glyphOffsetView = glyphOffsetBuffer->createView<uint32>(0, glyphOffsetBuffer->getSize());

		const DescriptorImage glyphDescriptorImage = {
			.imageView = glyphImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};
		const DescriptorImage glyphMapDescriptorImage = {
			.imageView = glyphMapImageView,
			.imageLayout = ImageLayout::GENERAL,
			.sampler = nullptr,
		};

		const sequence<WriteDescriptorSet> writeDescriptorSets = {
			// Glyph image
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = fontView,
			},
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = glyphView,
			},
			{
				.descriptorSet = glyphImageDescriptorSet,
				.bindingIndex = 2,
				.descriptorImages = glyphDescriptorImage,
			},

			// Glyph SDF
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = glyphDescriptorImage,
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = glyphMapDescriptorImage,
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = fontView,
			},
			{
				.descriptorSet = glyphSdfDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = glyphOffsetView,
			},
		};

		// TODO(asset-builder/sdf-generator/device): Obtain a device from LayerCompositor/GraphicsAdapter.
		Renderer::getDevice()->updateDescriptorSets(writeDescriptorSets);
	}
}