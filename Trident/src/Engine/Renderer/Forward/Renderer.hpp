// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/PerspectiveCamera.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class ForwardRenderer : public SceneRenderer<Scene3D> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
				vec3<float32> position;
			};

			struct Instance {
				mat4x4<float32> transform;
				mat3x3<float32> normal;
			};

		public:
			void build() override {
				_createRenderPass();

				colorImage = createColorImage();
				depthStencilImage = createDepthStencilImage();

				colorImageView = createColorImageView();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();

				vertexDescriptorSetLayout = createVertexDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({vertexDescriptorSetLayout});

				vertexDescriptorSet = descriptorSets[0];

				cameraBuffer = createCameraBuffer();
				cameraMappedRange = cameraBuffer->map(0, cameraBuffer->getSize());

				imageSampler = createImageSampler();
			}

			void postSetScene() override {
				createMaterialObjects();

				instanceBuffer = createInstanceBuffer();
				instanceMappedRange = instanceBuffer->map(0, instanceBuffer->getSize());

				Entity entity = 0;

				while (entity < scene->getInstanceCount()) {
					updateTransform(entity);

					entity += 1;
				}

				const view<uint8> sceneVertexBuffer = scene->getVertexBuffer();
				const ref<Buffer<uint8>> vertexStagingBuffer = createStagingBuffer<uint8>(sceneVertexBuffer.count());

				uint8* vertexMappedRange = vertexStagingBuffer->map(0, vertexStagingBuffer->getSize());

				std::copy(sceneVertexBuffer.begin(), sceneVertexBuffer.end(), vertexMappedRange);

				vertexStagingBuffer->unmap();

				vertexBuffer = createBuffer<uint8>({
					.size = vertexStagingBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Vertex",
					#endif
				});

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Copy vertex buffer",
					#endif
				});

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(
					vertexStagingBuffer,
					vertexBuffer,
					{
						{
							.sourceOffset = 0,
							.destinationOffset = 0,
							.size = vertexBuffer->getSize(),
						},
					}
				);

				queue->submit({commandEncoder->end()});
				queue->waitIdle();

				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);
				const ClearValue colorClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

				ClearValue depthStencilClearValue{};
				depthStencilClearValue.depth = 1.0f;
				depthStencilClearValue.stencil = 0;

				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getSwapchainImageIndex()];
				beginRenderPassDescriptor.clearValues = {colorClearValue, depthStencilClearValue, colorClearValue};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				// TODO(GL): Allow the vertex buffer to be bound before setting a render pipeline.

				const view<ref<Geometry3D>> geometries = scene->getGeometries();
				const view<RigidBody3D> instances = scene->getInstances();
				const std::vector<Shader>& materialShaders = scene->getMaterialShaders();
				const std::vector<std::vector<Material>>& materials = scene->getMaterials();

				uint32 instanceIndex = 0;

				while (instanceIndex < instances.count()) {
					const RigidBody3D& instance = instances[instanceIndex];
					const ref<Geometry3D> geometry = geometries[instance.geometryIndex];
					const uint32 materialShaderIndex = scene->getMaterialShaderIndex(instance.materialIndex);
					const uint32 vertexCount = static_cast<uint32>(geometry->getVertexPositionIndices().count());
					const uint32 indexOffset = scene->getVertexPositionIndexOffset(instance.geometryIndex);

					commandEncoder->bindGraphicsPipeline(materialPipelines[materialShaderIndex]);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 0, {vertexDescriptorSet, materialDescriptorSets[materialShaderIndex]});
					commandEncoder->draw(vertexCount, 1, indexOffset, instanceIndex);

					instanceIndex += 1;
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				colorImage = createColorImage();
				depthStencilImage = createDepthStencilImage();

				colorImageView = createColorImageView();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();
				updateCamera();
			}

			void updateTransform(Entity entity) {
				Instance& instance = instanceMappedRange[entity];
				instance.transform = scene->getInstances()[entity].transform;
				instance.normal = transpose(inverse(mat3x3<float32>(
					instance.transform[0][0], instance.transform[0][1], instance.transform[0][2],
					instance.transform[1][0], instance.transform[1][1], instance.transform[1][2],
					instance.transform[2][0], instance.transform[2][1], instance.transform[2][2]
				)));
			}

			void updateCamera() {
				const PerspectiveCamera& camera = getCameraController<PerspectiveCamera>().getCamera();

				cameraMappedRange->projectionView = camera.getProjectionView();
				cameraMappedRange->position = camera.getPosition();
			}

		private:
			void _createRenderPass() {
				static constexpr uint32 SAMPLE_COUNT = 4;

				Attachment colorAttachment{};
				colorAttachment.format = getSurface()->getFormat();
				colorAttachment.sampleCount = SAMPLE_COUNT;
				colorAttachment.loadOperation = LoadOperation::CLEAR;
				colorAttachment.storeOperation = StoreOperation::UNDEFINED;
				colorAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				Attachment depthStencilAttachment{};
				depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
				depthStencilAttachment.sampleCount = SAMPLE_COUNT;
				depthStencilAttachment.loadOperation = LoadOperation::CLEAR;
				depthStencilAttachment.storeOperation = StoreOperation::UNDEFINED;
				depthStencilAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				AttachmentReference colorAttachmentReference{};
				colorAttachmentReference.index = 0;
				colorAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				AttachmentReference depthStencilAttachmentReference{};
				depthStencilAttachmentReference.index = 1;
				depthStencilAttachmentReference.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				SubpassDescriptor subpassDescriptor{};
				subpassDescriptor.colorAttachmentReferences = {colorAttachmentReference};
				subpassDescriptor.depthStencilAttachmentReference = depthStencilAttachmentReference;

				RenderPassDescriptor renderPassDescriptor{};
				renderPassDescriptor.attachments = {colorAttachment, depthStencilAttachment};
				renderPassDescriptor.subpasses = {createSubpass(subpassDescriptor)};

				renderPass = createRenderPass(renderPassDescriptor);
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {colorImageView, depthStencilImageView, nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						SET_DEBUG_NAME(framebufferDescriptor, name);
					#endif

					framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			ref<Image> createColorImage() const {
				const Extent2D extent = getSwapchain()->getExtent();

				AttachmentImageDescriptor colorAttachmentImageDescriptor{};
				colorAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				colorAttachmentImageDescriptor.extent.width = extent.width;
				colorAttachmentImageDescriptor.extent.height = extent.height;
				colorAttachmentImageDescriptor.extent.depth = 1;
				colorAttachmentImageDescriptor.mipLevelCount = 1;
				colorAttachmentImageDescriptor.layerCount = 1;

				SET_DEBUG_NAME(colorAttachmentImageDescriptor, "Color (multisampled)");

				return renderPass->createColorAttachmentImage(colorAttachmentImageDescriptor, 0);
			}

			ref<Image> createDepthStencilImage() const {
				const Extent2D extent = getSwapchain()->getExtent();

				AttachmentImageDescriptor depthStencilAttachmentImageDescriptor{};
				depthStencilAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
				depthStencilAttachmentImageDescriptor.extent.width = extent.width;
				depthStencilAttachmentImageDescriptor.extent.height = extent.height;
				depthStencilAttachmentImageDescriptor.extent.depth = 1;
				depthStencilAttachmentImageDescriptor.mipLevelCount = 1;
				depthStencilAttachmentImageDescriptor.layerCount = 1;

				SET_DEBUG_NAME(depthStencilAttachmentImageDescriptor, "Depth stencil");

				return renderPass->createDepthStencilAttachmentImage(depthStencilAttachmentImageDescriptor, 0);
			}

			ref<ImageView> createColorImageView() const {
				return colorImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Color (multisampled)",
					#endif
				});
			}

			ref<ImageView> createDepthStencilImageView() const {
				return depthStencilImage->createView({
					.mipLevelOffset = 0,
					.mipLevelCount = 1,
					.layerOffset = 0,
					.layerCount = 1,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Depth stencil",
					#endif
				});
			}

			ref<DescriptorSetLayout> createVertexDescriptorSetLayout() const {
				return createDescriptorSetLayout({
					{
						.descriptorType = DescriptorType::UNIFORM_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::VERTEX,
					},
				});
			}

			ref<Buffer<Instance>> createInstanceBuffer() const {
				return createBuffer<Instance>({
					.size = scene->getInstanceCount() * sizeof(Instance),
					.usage = BufferUsage::VERTEX,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Instance",
					#endif
				});
			}

			void createMaterialObjects();

			ref<Buffer<Camera>> createCameraBuffer() const {
				return createBuffer<Camera>({
					.size = sizeof(Camera),
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = true,
					#if defined(BUILD_MODE_DEBUG)
						.name = "Camera",
					#endif
				});
			}

			ref<Sampler> createImageSampler() const {
				return createSampler({
					.magFilter = SamplerFilter::LINEAR,
					.minFilter = SamplerFilter::LINEAR,
				});
			}

			void updateDescriptorSets() const {
				DescriptorBuffer cameraDescriptorBuffer{};
				cameraDescriptorBuffer.buffer = cameraBuffer;
				cameraDescriptorBuffer.offset = 0;
				cameraDescriptorBuffer.size = cameraBuffer->getSize();

				DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
				vertexPositionComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexPositionComponentDescriptorBuffer.offset = 0;
				vertexPositionComponentDescriptorBuffer.size = scene->getVertexPositionComponentBufferSize();

				DescriptorBuffer vertexPositionIndexDescriptorBuffer{};
				vertexPositionIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexPositionIndexDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize();
				vertexPositionIndexDescriptorBuffer.size = scene->getVertexPositionIndexBufferSize();

				DescriptorBuffer vertexNormalComponentDescriptorBuffer{};
				vertexNormalComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexNormalComponentDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize() + scene->getVertexPositionIndexBufferSize();
				vertexNormalComponentDescriptorBuffer.size = scene->getVertexNormalComponentBufferSize();

				DescriptorBuffer vertexNormalIndexDescriptorBuffer{};
				vertexNormalIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexNormalIndexDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize() + scene->getVertexPositionIndexBufferSize() + scene->getVertexNormalComponentBufferSize();
				vertexNormalIndexDescriptorBuffer.size = scene->getVertexNormalIndexBufferSize();

				DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer{};
				vertexTextureCoordinateComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexTextureCoordinateComponentDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize() + scene->getVertexPositionIndexBufferSize() + scene->getVertexNormalComponentBufferSize() + scene->getVertexNormalIndexBufferSize();
				vertexTextureCoordinateComponentDescriptorBuffer.size = scene->getVertexTextureCoordinateComponentBufferSize();

				DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer{};
				vertexTextureCoordinateIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexTextureCoordinateIndexDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize() + scene->getVertexPositionIndexBufferSize() + scene->getVertexNormalComponentBufferSize() + scene->getVertexNormalIndexBufferSize() + scene->getVertexTextureCoordinateComponentBufferSize();
				vertexTextureCoordinateIndexDescriptorBuffer.size = scene->getVertexTextureCoordinateIndexBufferSize();

				DescriptorBuffer vertexTangentComponentDescriptorBuffer{};
				vertexTangentComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexTangentComponentDescriptorBuffer.offset = scene->getVertexPositionComponentBufferSize() + scene->getVertexPositionIndexBufferSize() + scene->getVertexNormalComponentBufferSize() + scene->getVertexNormalIndexBufferSize() + scene->getVertexTextureCoordinateComponentBufferSize() + scene->getVertexTextureCoordinateIndexBufferSize();
				vertexTangentComponentDescriptorBuffer.size = scene->getVertexTangentComponentBufferSize();

				WriteDescriptorSet cameraWriteDescriptorSet{};
				cameraWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				cameraWriteDescriptorSet.bindingIndex = 0;
				cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

				WriteDescriptorSet vertexPositionComponentWriteDescriptorSet{};
				vertexPositionComponentWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexPositionComponentWriteDescriptorSet.bindingIndex = 1;
				vertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

				WriteDescriptorSet vertexPositionIndexWriteDescriptorSet{};
				vertexPositionIndexWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexPositionIndexWriteDescriptorSet.bindingIndex = 2;
				vertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

				WriteDescriptorSet vertexNormalComponentWriteDescriptorSet{};
				vertexNormalComponentWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexNormalComponentWriteDescriptorSet.bindingIndex = 3;
				vertexNormalComponentWriteDescriptorSet.descriptorBuffers = {vertexNormalComponentDescriptorBuffer};

				WriteDescriptorSet vertexNormalIndexWriteDescriptorSet{};
				vertexNormalIndexWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexNormalIndexWriteDescriptorSet.bindingIndex = 4;
				vertexNormalIndexWriteDescriptorSet.descriptorBuffers = {vertexNormalIndexDescriptorBuffer};

				WriteDescriptorSet vertexTextureCoordinateComponentWriteDescriptorSet{};
				vertexTextureCoordinateComponentWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexTextureCoordinateComponentWriteDescriptorSet.bindingIndex = 5;
				vertexTextureCoordinateComponentWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer};

				WriteDescriptorSet vertexTextureCoordinateIndexWriteDescriptorSet{};
				vertexTextureCoordinateIndexWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexTextureCoordinateIndexWriteDescriptorSet.bindingIndex = 6;
				vertexTextureCoordinateIndexWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateIndexDescriptorBuffer};

				WriteDescriptorSet vertexTangentComponentWriteDescriptorSet{};
				vertexTangentComponentWriteDescriptorSet.descriptorSet = vertexDescriptorSet;
				vertexTangentComponentWriteDescriptorSet.bindingIndex = 7;
				vertexTangentComponentWriteDescriptorSet.descriptorBuffers = {vertexTangentComponentDescriptorBuffer};

				getDevice()->updateDescriptorSets({
					cameraWriteDescriptorSet,
					vertexPositionComponentWriteDescriptorSet,
					vertexPositionIndexWriteDescriptorSet,
					vertexNormalComponentWriteDescriptorSet,
					vertexNormalIndexWriteDescriptorSet,
					vertexTextureCoordinateComponentWriteDescriptorSet,
					vertexTextureCoordinateIndexWriteDescriptorSet,
					vertexTangentComponentWriteDescriptorSet,
				});
			}

			void postSetCameraController() override {}

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> vertexDescriptorSetLayout;
			sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts;

			ref<DescriptorSet> vertexDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;

			sequence<ref<PipelineLayout>> materialPipelineLayouts;

			sequence<ref<GraphicsPipeline>> materialPipelines;

			ref<Buffer<uint8>> vertexBuffer;
			ref<Buffer<Instance>> instanceBuffer;
			ref<Buffer<Camera>> cameraBuffer;

			ref<Image> colorImage;
			ref<Image> depthStencilImage;
			sequence<sequence<ref<Image>>> materialImages;

			ref<ImageView> colorImageView;
			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;

			ref<Sampler> imageSampler;

			Camera* cameraMappedRange = nullptr;
			Instance* instanceMappedRange = nullptr;
	};
}