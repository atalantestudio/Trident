// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/Font/GlyphRasterizer.hpp"

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

			const vec2<uint32> glyphSize = vec2<uint32>(ceil(spreadGlyphTexelSize));

			x += glyphSize.x;
			rowHeight = max(rowHeight, glyphSize.y);

			if (x >= glyphMapExtent.x) {
				x = 0;
				y += rowHeight;
				rowHeight = 0;
			}

			if (y >= glyphMapExtent.y) {
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

		return sizeof(GlyphRasterizer::GlyphBuffer) + maxGlyphEdgeCount * sizeof(GlyphRasterizer::GlyphEdge);
	}

	void GlyphRasterizer::initialize(const GraphicsAdapter* adapter) {
		this->adapter = adapter;

		instance = adapter->createInstance({
			.projectName = "Glyph rasterizer",
			.projectVersion = {0, 0, 1},
		});
		physicalDevice = instance->selectPhysicalDevice(false);
		device = physicalDevice->createDevice<>();
	}

	void GlyphRasterizer::generateGlyphAtlas(const File<FileFormat::TTF>& file, SDFGlyphMap& output) {
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

		fontBuffer = createFontBuffer();
		glyphBuffer = createGlyphBuffer(glyphBufferSize);
		glyphOffsetBuffer = createGlyphOffsetBuffer();
		glyphImage = createGlyphImage();
		glyphMapImage = createGlyphMapImage(output.extent);
		glyphImageView = createGlyphImageView();
		glyphMapImageView = createGlyphMapImageView();
		glyphImageDescriptorSetLayout = createGlyphImageDescriptorSetLayout();
		glyphSdfDescriptorSetLayout = createGlyphSdfDescriptorSetLayout();

		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			glyphImageDescriptorSetLayout,
			glyphSdfDescriptorSetLayout,
		};

		// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
		sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets(descriptorSetLayouts);

		glyphImageDescriptorSet = descriptorSets[0];
		glyphSdfDescriptorSet = descriptorSets[1];

		updateDescriptorSets();

		glyphImagePipelineLayout = createGlyphImagePipelineLayout();
		glyphSdfPipelineLayout = createGlyphSdfPipelineLayout();
		glyphImageComputePipeline = createGlyphImageComputePipeline();
		glyphSdfComputePipeline = createGlyphSdfComputePipeline();

		const ref<Buffer> fontStagingBuffer = device->createDynamicBuffer({
			.size = fontBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Font staging",
			#endif
		});
		const BufferView<FontStorage> fontStagingView = fontStagingBuffer->createView<FontStorage>(0, fontStagingBuffer->getSize());

		FontStorage* fontStagingBufferMappedRange = fontStagingView.map();
		fontStagingBufferMappedRange->spreadFactor = spreadFactor;
		fontStagingBufferMappedRange->outputGlyphSize = GLYPH_SUBIMAGE_DIMENSION;
		fontStagingBufferMappedRange->lineHeight = static_cast<sint32>(lineHeight);

		fontStagingBuffer->unmap();

		const BufferCopyRegion fontBufferCopyRegion = {
			.sourceOffset = 0,
			.destinationOffset = 0,
			.size = fontBuffer->getSize(),
		};

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::COMPUTE);

		ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Generate glyph map",
			#endif
		});
		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(fontStagingBuffer, fontBuffer, fontBufferCopyRegion);

		const ImageMemoryBarrier imageMemoryBarriers[] = {
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

		const BufferView<uint8> glyphView = glyphBuffer->createView<uint8>(0, glyphBuffer->getSize());
		const BufferView<uint32> glyphOffsetView = glyphOffsetBuffer->createView<uint32>(0, glyphOffsetBuffer->getSize());

		GlyphBuffer* glyphMappedRange = pointer_cast<GlyphBuffer*>(glyphView.map());
		uint32* glyphOffsetMappedRange = glyphOffsetView.map();

		const ref<Fence> writeBufferFence = device->createFence({
			.signaled = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Buffer write",
			#endif
		});
		const Extent3D& glyphMapExtent = glyphMapImage->getExtent();
		const float32 inverseLineHeight = static_cast<float32>(1.0 / lineHeight);
		const float32 scale = inverseLineHeight * static_cast<float32>(GLYPH_SUBIMAGE_DIMENSION);
		const vec2<float32> inverseGlyphMapSize = 1.0f / vec2<float32>(vec2<uint32>{glyphMapExtent.x, glyphMapExtent.y});

		uint32 x = 0;
		uint32 y = 0;
		uint32 rowHeight = 0;
		uint64 glyphIndex = 0;
		uint16 edgeIndex;

		while (glyphIndex < glyphs.count()) {
			const TrueTypeFont::Glyph& glyph = glyphs[glyphIndex];

			// TODO(asset-builder/font/ttf/metric-overflow)
			// On some fonts (e.g. Bodoni Black) there is one less hMetric entry.
			// For now, overflowing glyphs use the last entry. This seems coherent with FontDrop.
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

			const vec2<uint32> glyphSize = vec2<uint32>(ceil(spreadGlyphTexelSize));

			if (x + glyphSize.x >= glyphMapExtent.x) {
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

			// TODO(asset-builder/font/sdf): Dispatch less groups depending on the size of the area occupied by the glyph.
			commandEncoder->dispatch(128, 128, 1);

			if (adapter->getApi() == GraphicsAPI::DX12) {
				const ImageMemoryBarrier glyphImageMemoryBarrier = {
					.image = glyphImage,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, glyphImageMemoryBarrier);
			}

			commandEncoder->bindComputePipeline(glyphSdfComputePipeline);
			commandEncoder->bindDescriptorSets(glyphSdfPipelineLayout, 0, glyphSdfDescriptorSet);
			commandEncoder->dispatch(8, 8, 1);

			commandBuffer = commandEncoder->end();

			queue->submit(commandBuffer, nullptr, nullptr, writeBufferFence);
			queue->waitIdle();

			outputGlyph.minCoordinate.x = (x + spreadFactor) * inverseGlyphMapSize.x;
			outputGlyph.minCoordinate.y = (y + spreadFactor) * inverseGlyphMapSize.y;
			outputGlyph.maxCoordinate.x = (x + spreadFactor + rawGlyphTexelSize[0] + 1) * inverseGlyphMapSize.x;
			outputGlyph.maxCoordinate.y = (y + spreadFactor + rawGlyphTexelSize[1] + 1) * inverseGlyphMapSize.y;

			x += glyphSize.x;
			rowHeight = max(rowHeight, glyphSize.y);
			glyphIndex += 1;
		}

		glyphBuffer->unmap();
		glyphOffsetBuffer->unmap();

		const ref<Buffer> glyphMapStagingBuffer = device->createDynamicBuffer({
			.size = output.extent.x * output.extent.y,
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
			.imageOffset = {0, 0, 0},
			.imageExtent = glyphMapImage->getExtent(),
			.imageMipLevel = glyphMapImageView->getMipLevelOffset(),
			.imageLayerOffset = glyphMapImageView->getLayerOffset(),
			.imageLayerCount = glyphMapImageView->getLayerCount(),
		};

		commandEncoder->copyImageToBuffer(glyphMapImage, glyphMapStagingBuffer, glyphMapBufferImageCopyRegion);

		commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		const BufferView<uint8> glyphMapView = glyphMapStagingBuffer->createView<uint8>(0, glyphMapStagingBuffer->getSize());

		output.data = sequence<uint8>(output.extent.x * output.extent.y);

		std::copy_n(glyphMapView.map(), glyphMapStagingBuffer->getSize(), &output.data[0]);

		glyphMapStagingBuffer->unmap();
	}

	ref<ShaderModule> GlyphRasterizer::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (adapter->getApi()) {
			case GraphicsAPI::DX12: {
				pathString += ".dxil";

				break;
			}
			case GraphicsAPI::GL: {
				#if defined(GL_SOURCE_FROM_SPIRV)
					pathString += ".spv";
				#else
					pathString += ".glsl";
				#endif

				break;
			}
			case GraphicsAPI::VK: {
				pathString += ".spv";

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		path = view<char8>(&pathString[0], pathString.size());

		const sequence<uint8> code = Filesystem::openByteBuffer(path);

		return device->createShaderModule(descriptor, code);
	}

	ref<Buffer> GlyphRasterizer::createFontBuffer() const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceilPowerOf2(sizeof(FontStorage), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Font",
			#endif
		});
	}

	ref<Buffer> GlyphRasterizer::createGlyphBuffer(uint64 size) const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceilPowerOf2(size, alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph",
			#endif
		});
	}

	ref<Buffer> GlyphRasterizer::createGlyphOffsetBuffer() const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceilPowerOf2(3 * sizeof(float32), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph offset",
			#endif
		});
	}

	ref<Image> GlyphRasterizer::createGlyphImage() const {
		return device->createDynamicImage({
			.type = ImageType::TYPE_2D,
			.format = ImageFormat::R8_UNORM,
			.extent = {4096, 4096, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = ImageSampleCount::x1,
			.usage = ImageUsage::TRANSFER_SRC | ImageUsage::STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph",
			#endif
		});
	}

	ref<Image> GlyphRasterizer::createGlyphMapImage(Extent2D extent) const {
		return device->createDynamicImage({
			.type = ImageType::TYPE_2D,
			.format = ImageFormat::R8_UNORM,
			.extent = {extent.x, extent.y, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = ImageSampleCount::x1,
			.usage = ImageUsage::TRANSFER_SRC | ImageUsage::STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map",
			#endif
		});
	}

	ref<ImageView> GlyphRasterizer::createGlyphImageView() const {
		return glyphImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph view",
			#endif
		});
	}

	ref<ImageView> GlyphRasterizer::createGlyphMapImageView() const {
		return glyphMapImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph map",
			#endif
		});
	}

	ref<DescriptorSetLayout> GlyphRasterizer::createGlyphImageDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Font
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Glyph
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Read-write glyph
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return device->createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph image",
			#endif
		});
	}

	ref<DescriptorSetLayout> GlyphRasterizer::createGlyphSdfDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Read-only glyph
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Glyph map
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Font
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Glyph offset
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return device->createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph SDF",
			#endif
		});
	}

	ref<PipelineLayout> GlyphRasterizer::createGlyphImagePipelineLayout() const {
		return device->createPipelineLayout({
			.descriptorSetLayouts = {glyphImageDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph image",
			#endif
		});
	}

	ref<PipelineLayout> GlyphRasterizer::createGlyphSdfPipelineLayout() const {
		return device->createPipelineLayout({
			.descriptorSetLayouts = {glyphSdfDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Glyph SDF",
			#endif
		});
	}

	ref<ComputePipeline> GlyphRasterizer::createGlyphImageComputePipeline() const {
		return device->createComputePipeline({
			.pipelineLayout = glyphImagePipelineLayout,
			.computeShaderModule = loadShaderModule(
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

	ref<ComputePipeline> GlyphRasterizer::createGlyphSdfComputePipeline() const {
		return device->createComputePipeline({
			.pipelineLayout = glyphSdfPipelineLayout,
			.computeShaderModule = loadShaderModule(
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

	void GlyphRasterizer::updateDescriptorSets() const {
		const BufferView<FontStorage> fontView = fontBuffer->createView<FontStorage>(0, fontBuffer->getSize());
		const BufferView<uint8> glyphView = glyphBuffer->createView<uint8>(0, glyphBuffer->getSize());
		const BufferView<uint32> glyphOffsetView = glyphOffsetBuffer->createView<uint32>(0, glyphOffsetBuffer->getSize());

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

		const WriteDescriptorSet writeDescriptorSets[] = {
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

		device->updateDescriptorSets(writeDescriptorSets);
	}
}