#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<Image> SDFGlyphMapBuildProcess::build(const TTFFile& ttfFile) {
		ENGINE_ASSERT(ttfFile.glyphs.size() < 4096, "Too much glyphs to compute.");

		const uint64 maxGlyphSize = computeMaxGlyphSize(ttfFile);

		BufferDescriptor glyphStagingBufferDescriptor{};
		glyphStagingBufferDescriptor.size = maxGlyphSize;
		glyphStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> glyphStagingBuffer = Renderer::getBackend()->createBuffer(glyphStagingBufferDescriptor);

		BufferDescriptor glyphStorageBufferDescriptor{};
		glyphStorageBufferDescriptor.size = maxGlyphSize;
		glyphStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphStorageBuffer = Renderer::getBackend()->createBuffer(glyphStorageBufferDescriptor);

		BufferDescriptor glyphIndexStagingBufferDescriptor{};
		glyphIndexStagingBufferDescriptor.size = sizeof(uint32);
		glyphIndexStagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;

		const ref<Buffer> glyphIndexStagingBuffer = Renderer::getBackend()->createBuffer(glyphIndexStagingBufferDescriptor);

		BufferDescriptor glyphIndexStorageBufferDescriptor{};
		glyphIndexStorageBufferDescriptor.size = sizeof(uint32);
		glyphIndexStorageBufferDescriptor.usage = BufferUsage::TRANSFER_DST | BufferUsage::STORAGE;

		const ref<Buffer> glyphIndexStorageBuffer = Renderer::getBackend()->createBuffer(glyphIndexStorageBufferDescriptor);

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

		DescriptorSetLayoutBinding glyphBufferDescriptorSetLayoutBinding;
		glyphBufferDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_BUFFER;
		glyphBufferDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphBufferDescriptorSetLayoutBinding.buffers = {glyphStorageBuffer};

		DescriptorSetLayoutBinding glyphImageDescriptorSetLayoutBinding;
		glyphImageDescriptorSetLayoutBinding.descriptorType = DescriptorType::STORAGE_IMAGE;
		glyphImageDescriptorSetLayoutBinding.stageFlags = ShaderStage::COMPUTE;
		glyphImageDescriptorSetLayoutBinding.imageViews = {glyphImageView};

		DescriptorSetLayout glyphImageDescriptorSetLayout;
		glyphImageDescriptorSetLayout.bindings = {glyphBufferDescriptorSetLayoutBinding, glyphImageDescriptorSetLayoutBinding};

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

		ImageDescriptor glyphMapSdfImageDescriptor{};
		glyphMapSdfImageDescriptor.type = ImageType::TEXTURE_2D;
		glyphMapSdfImageDescriptor.format = ImageFormat::R8_UNORM;
		glyphMapSdfImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE;
		glyphMapSdfImageDescriptor.width = 512;
		glyphMapSdfImageDescriptor.height = 512;
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
		glyphIndexDescriptorSetLayoutBinding.buffers = {glyphIndexStorageBuffer};

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

		// Build process
		{
			const ref<Device> device = Renderer::getBackend()->getDevice();

			CommandBufferDescriptor commandBufferDescriptor{};
			commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

			for (uint16 glyphIndex = 0, glyphOffset = 0; glyphIndex < 1; glyphIndex += 1) {
				ENGINE_TRACE("Computing SDF for glyph [].", glyphIndex);

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

				// TODO: Use persistent mapping for this buffer.
				uint32* glyphIndexMappedRange = glyphIndexStagingBuffer->map<uint32>(0, sizeof(uint32));

				*glyphIndexMappedRange = glyphOffset;

				glyphIndexStagingBuffer->unmap();

				{
					const ref<CommandEncoder> commandEncoder = device->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

					commandEncoder->begin();
					commandEncoder->copyBufferToBuffer(glyphIndexStagingBuffer, glyphIndexStorageBuffer, 0, 0, sizeof(uint32));

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

				glyphOffset += 1;
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

	uint64 SDFGlyphMapBuildProcess::computeMaxGlyphSize(const TTFFile& ttfFile) {
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