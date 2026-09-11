// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/Renderer/Debug/CameraBuffer.hpp"
#include "Engine/Renderer/Debug/DepthStencilImage.hpp"
#include "Engine/Renderer/Debug/DepthStencilImageView.hpp"
#include "Engine/Renderer/Debug/Framebuffer.hpp"
#include "Engine/Renderer/Debug/GeometryDescriptorSetLayout.hpp"
#include "Engine/Renderer/Debug/GeometryFillPipeline.hpp"
#include "Engine/Renderer/Debug/GeometryLinePipeline.hpp"
#include "Engine/Renderer/Debug/GeometryPipelineLayout.hpp"
#include "Engine/Renderer/Debug/IndexBuffer.hpp"
#include "Engine/Renderer/Debug/Renderer.hpp"
#include "Engine/Renderer/Debug/RenderPass.hpp"
#include "Engine/Renderer/Debug/SkyboxCameraBuffer.hpp"
#include "Engine/Renderer/Debug/SkyboxDescriptorSetLayout.hpp"
#include "Engine/Renderer/Debug/SkyboxImage.hpp"
#include "Engine/Renderer/Debug/SkyboxImageView.hpp"
#include "Engine/Renderer/Debug/SkyboxPipeline.hpp"
#include "Engine/Renderer/Debug/SkyboxPipelineLayout.hpp"
#include "Engine/Renderer/Debug/SkyboxSampler.hpp"
#include "Engine/Renderer/Debug/VertexBuffer.hpp"

namespace Trident {
	void DebugRenderer::build() {
		renderPass = createRenderPass();
		skyboxDescriptorSetLayout = createSkyboxDescriptorSetLayout();
		geometryDescriptorSetLayout = createGeometryDescriptorSetLayout();

		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			skyboxDescriptorSetLayout,
			geometryDescriptorSetLayout,
		};

		descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

		skyboxPipelineLayout = createSkyboxPipelineLayout();
		geometryPipelineLayout = createGeometryPipelineLayout();
		skyboxPipeline = createSkyboxPipeline();
		geometryFillPipeline = createGeometryFillPipeline();
		geometryLinePipeline = createGeometryLinePipeline();
		skyboxCameraBuffer = createSkyboxCameraBuffer();
		cameraBuffer = createCameraBuffer();
		skyboxSampler = createSkyboxSampler();

		skyboxCameraView = skyboxCameraBuffer->createView<SkyboxCamera>(0, skyboxCameraBuffer->getSize());
		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

		skyboxCameraMappedRange = skyboxCameraView.map();
		cameraMappedRange = cameraView.map();

		createFrameObjects();
	}

	void DebugRenderer::postSetScene() {
		vertexBuffer = createVertexBuffer();
		indexBuffer = createIndexBuffer();

		if (scene->skyboxImageAsset != nullptr) {
			skyboxImage = createSkyboxImage(scene->skyboxImageAsset);
			skyboxImageView = createSkyboxImageView();
		}

		updateDescriptorSets();
		encodeSceneOneTimeCommands();
	}

	void DebugRenderer::createFrameObjects() {
		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();
	}

	void DebugRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
		const Extent2D extent = framebuffer->getExtent();
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffer,
			.clearValues = {
				{0.0f, 0.0f, 0.0f, 1.0f},
				{
					.depth = 0.0f,
					.stencil = 0,
				},
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);

		if (scene->skyboxImageAsset != nullptr) {
			encodeSkyboxPass(commandEncoder);
		}

		encodeGeometryFillPass(commandEncoder);
		encodeGeometryLinePass(commandEncoder);

		commandEncoder->endRenderPass();
	}

	void DebugRenderer::resize(WindowResizeEvent& event) {
		createFrameObjects();
	}

	void DebugRenderer::updateCamera() const {
		const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();

		cameraMappedRange->projectionView = camera.getProjectionView();
		cameraMappedRange->position = svec3<float32>(camera.position);

		mat4x4<float32> skyboxView = camera.getView();
		skyboxView[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		skyboxCameraMappedRange->projectionView = skyboxView * camera.getProjection();
	}

	void DebugRenderer::updateDescriptorSets() const {
		const WriteDescriptorSet writeDescriptorSets[] = {
			// Camera
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);

		if (scene->skyboxImageAsset != nullptr) {
			const DescriptorImage skyboxDescriptorImage = {
				.imageView = skyboxImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = skyboxSampler,
			};

			const WriteDescriptorSet writeDescriptorSets[] = {
				// Skybox camera
				{
					.descriptorSet = descriptorSets[0],
					.bindingIndex = 0,
					.descriptorBufferViews = skyboxCameraView,
				},

				// Skybox
				{
					.descriptorSet = descriptorSets[0],
					.bindingIndex = 1,
					.descriptorImages = skyboxDescriptorImage,
				},
			};

			getDevice()->updateDescriptorSets(writeDescriptorSets);
		}
	}

	void DebugRenderer::encodeSceneOneTimeCommands() const {
		uint64 stagingBufferSize = scene->vertexRange.count + scene->indexRange.count + scene->faceLineIndexRange.count;

		if (scene->skyboxImageAsset != nullptr) {
			stagingBufferSize += skyboxImage->getSize();
		}

		const ref<Buffer> stagingBuffer = createDynamicBuffer({
			.size = stagingBufferSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_COHERENT | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});

		BufferView<uint8> vertexView;
		BufferView<uint8> indexView;
		BufferView<uint8> skyboxView;
		uint64 viewOffset = 0;

		vertexView = stagingBuffer->createView<uint8>(viewOffset, scene->vertexRange.count);

		std::copy_n(&scene->geometryData[scene->vertexRange.offset], vertexView.getSize(), vertexView.map());

		viewOffset += vertexView.getSize();
		indexView = stagingBuffer->createView<uint8>(viewOffset, scene->indexRange.count + scene->faceLineIndexRange.count);

		std::copy_n(&scene->geometryData[scene->indexRange.offset], indexView.getSize(), indexView.map());

		viewOffset += indexView.getSize();

		if (scene->skyboxImageAsset != nullptr) {
			skyboxView = stagingBuffer->createView<uint8>(viewOffset, skyboxImage->getSize());

			const view<uint8> data = scene->skyboxImageAsset->getData();

			if (scene->skyboxImageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
				convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, skyboxView.map(), skyboxView.getSize());
			}
			else {
				std::copy(data.begin(), data.end(), skyboxView.map());
			}

			viewOffset += skyboxView.getSize();
		}

		stagingBuffer->unmap();

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scene one-time commands",
			#endif
		});

		commandEncoder->begin();

		const BufferCopyRegion bufferCopyRegions[] = {
			{
				.sourceOffset = vertexView.getOffset(),
				.destinationOffset = 0,
				.size = vertexView.getSize(),
			},
			{
				.sourceOffset = indexView.getOffset(),
				.destinationOffset = 0,
				.size = indexView.getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[1]);

		if (scene->skyboxImageAsset != nullptr) {
			ImageMemoryBarrier imageMemoryBarrier = {
				.image = skyboxImage,
				.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
				.newAccess = Access::TRANSFER_WRITE,
			};

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

			const BufferImageCopyRegion bufferImageCopyRegion = {
				.bufferOffset = skyboxView.getOffset(),
				.imageOffsetX = 0,
				.imageOffsetY = 0,
				.imageOffsetZ = 0,
				.imageExtent = skyboxImage->getExtent(),
				.imageMipLevel = skyboxImageView->getMipLevelOffset(),
				.imageLayerOffset = skyboxImageView->getLayerOffset(),
				.imageLayerCount = skyboxImageView->getLayerCount(),
			};

			commandEncoder->copyBufferToImage(stagingBuffer, skyboxImage, bufferImageCopyRegion);

			imageMemoryBarrier.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
			imageMemoryBarrier.newAccess = Access::SHADER_READ;

			commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}

	void DebugRenderer::encodeSkyboxPass(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(skyboxPipeline);
		commandEncoder->bindDescriptorSets(skyboxPipelineLayout, 0, descriptorSets[0]);
		commandEncoder->draw(36, 1, 0, 0);
	}

	void DebugRenderer::encodeGeometryFillPass(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(geometryFillPipeline);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
		commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
		commandEncoder->bindDescriptorSets(geometryPipelineLayout, 0, descriptorSets[1]);

		const uint32 indexCount = static_cast<uint32>(scene->indexRange.count / sizeof(uint32));

		commandEncoder->drawIndexed(indexCount, 1, 0, 0, 0);
	}

	void DebugRenderer::encodeGeometryLinePass(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(geometryLinePipeline);
		commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
		commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, scene->indexRange.count);
		commandEncoder->bindDescriptorSets(geometryPipelineLayout, 0, descriptorSets[1]);

		const uint32 indexCount = static_cast<uint32>(scene->faceLineIndexRange.count / sizeof(uint32));

		commandEncoder->drawIndexed(indexCount, 1, 0, 0, 0);
	}
}