// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"

namespace Trident {
	class StaticSceneRayTracer : public BaseSceneRenderer {
		public:
			// Called on startup and on window resize.
			void initialize() override {
				accumulateBuffer = createAccumulateBuffer();

				outputImage = createOutputImage();
				outputImageView = createOutputImageView();

				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor = {
					.renderPass = presentRenderPass,
					.imageViews = {nullptr},
					.extent = swapchain->getExtent(),
				};

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(DEBUG_OBJECT_NAME)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}

				if (getGraphicsApi() != GraphicsAPI::DX12) {
					// TODO: Avoid creating a command encoder for a single image transition?
					const ref<Queue> graphicsQueue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
					const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
						.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Transition output image",
						#endif
					});

					commandEncoder->begin();

					const ImageMemoryBarrier outputImageMemoryBarrier = {
						.image = outputImage,
						.newLayout = ImageLayout::GENERAL,
						.newAccess = Access::SHADER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, outputImageMemoryBarrier);

					const ref<CommandBuffer> commandBuffer = commandEncoder->end();

					graphicsQueue->submit(commandBuffer);
					graphicsQueue->waitIdle();
				}

				updateImageDescriptorSets();
			}

			void build() override {
				presentRenderPass = createPresentRenderPass();

				rayTracingDescriptorSetLayout = createRayTracingDescriptorSetLayout();
				presentDescriptorSetLayout = createPresentDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(sequence<ref<DescriptorSetLayout>>{
					rayTracingDescriptorSetLayout,
					presentDescriptorSetLayout,
				});

				rayTracingDescriptorSet = descriptorSets[0];
				presentDescriptorSet = descriptorSets[1];

				initialize();

				cameraBuffer = createCameraBuffer();
				cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

				rayTracingPipelineLayout = createRayTracingPipelineLayout();
				presentPipelineLayout = createPresentPipelineLayout();

				rayTracingPipeline = createRayTracingPipeline();
				presentPipeline = createPresentPipeline();
			}

			void postSetScene() override {
				staticBuffer = createStaticBuffer();

				copyBuffers();
				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				cameraMappedRange->frameIndex += 1;

				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);
				const uint32 groupCountX = static_cast<uint32>(ceil(width / 16.0f));
				const uint32 groupCountY = static_cast<uint32>(ceil(height / 16.0f));

				commandEncoder->bindComputePipeline(rayTracingPipeline);
				commandEncoder->bindDescriptorSets(rayTracingPipelineLayout, 0, rayTracingDescriptorSet);
				commandEncoder->dispatch(groupCountX, groupCountY, 1);

				commandEncoder->beginRenderPass({
					.framebuffer = framebuffers[getSwapchainImageIndex()],
					.clearValues = {
						{0.0f, 0.0f, 0.0f, 1.0f},
					},
				});
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);
				commandEncoder->bindGraphicsPipeline(presentPipeline);
				commandEncoder->bindDescriptorSets(presentPipelineLayout, 0, presentDescriptorSet);
				commandEncoder->draw(3, 1, 0, 0);
				commandEncoder->endRenderPass();
			}

			void resetFrameIndex() const {
				cameraMappedRange->frameIndex = 0;
			}

		private:
			ref<RenderPass> createPresentRenderPass() const;

			ref<DescriptorSetLayout> createRayTracingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPresentDescriptorSetLayout() const;

			ref<PipelineLayout> createRayTracingPipelineLayout() const;
			ref<PipelineLayout> createPresentPipelineLayout() const;

			ref<ComputePipeline> createRayTracingPipeline() const;
			ref<GraphicsPipeline> createPresentPipeline() const;

			ref<Buffer<uint8>> createAccumulateBuffer() const;

			ref<Image> createOutputImage() const;
			ref<ImageView> createOutputImageView();

			void updateDescriptorSets() const {
				const ref<Device> device = getDevice();
				const ref<Scene3DAsset> sceneAsset = scene->getAsset();
				const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
				const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
				const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
				const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
				const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
				const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
				const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
				const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);
				const uint64 textureCoordinateAlignedRangeSize = ceil(textureCoordinateRangeSize, minStorageBufferOffsetAlignment);

				const sequence<DescriptorBuffer> descriptorBuffers = {
					// Camera
					{
						.buffer = cameraBuffer,
						.offset = 0,
						.size = cameraBuffer->getSize(),
					},

					// Static
					{
						.buffer = staticBuffer,
						.offset = 0,
						.size = triangleRangeSize,
					},
					{
						.buffer = staticBuffer,
						.offset = triangleAlignedRangeSize,
						.size = vertexRangeSize,
					},
					{
						.buffer = staticBuffer,
						.offset = triangleAlignedRangeSize + vertexAlignedRangeSize,
						.size = normalRangeSize,
					},
					{
						.buffer = staticBuffer,
						.offset = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize,
						.size = textureCoordinateRangeSize,
					},
				};

				sequence<WriteDescriptorSet> writeDescriptorSets(5);
				uint32 writeDescriptorSetIndex = 0;

				while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
					writeDescriptorSets[writeDescriptorSetIndex] = {
						.descriptorSet = rayTracingDescriptorSet,
						.bindingIndex = writeDescriptorSetIndex,
						.descriptorBuffers = descriptorBuffers[writeDescriptorSetIndex],
					};

					writeDescriptorSetIndex += 1;
				}

				device->updateDescriptorSets(writeDescriptorSets);
			}

			void updateImageDescriptorSets() {
				const DescriptorBuffer accumulateDescriptorBuffer = {
					.buffer = accumulateBuffer,
					.offset = 0,
					.size = accumulateBuffer->getSize(),
				};
				const DescriptorImage outputDescriptorImage = {
					.imageView = outputImageView,
					.imageLayout = ImageLayout::GENERAL,
					.sampler = nullptr,
				};

				const sequence<WriteDescriptorSet> writeDescriptorSets = {
					{
						.descriptorSet = rayTracingDescriptorSet,
						.bindingIndex = 5,
						.descriptorBuffers = accumulateDescriptorBuffer,
					},
					{
						.descriptorSet = rayTracingDescriptorSet,
						.bindingIndex = 6,
						.descriptorImages = outputDescriptorImage,
					},
					{
						.descriptorSet = presentDescriptorSet,
						.bindingIndex = 0,
						.descriptorImages = outputDescriptorImage,
					},
				};

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

		private:
			ref<RenderPass> presentRenderPass;
			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> rayTracingDescriptorSetLayout;
			ref<DescriptorSetLayout> presentDescriptorSetLayout;

			ref<DescriptorSet> rayTracingDescriptorSet;
			ref<DescriptorSet> presentDescriptorSet;

			ref<PipelineLayout> rayTracingPipelineLayout;
			ref<PipelineLayout> presentPipelineLayout;

			ref<ComputePipeline> rayTracingPipeline;
			ref<GraphicsPipeline> presentPipeline;

			ref<Buffer<uint8>> accumulateBuffer;

			ref<Image> outputImage;
			ref<ImageView> outputImageView;
	};
}