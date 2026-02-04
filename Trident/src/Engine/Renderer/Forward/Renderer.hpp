// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class ForwardRenderer : public SceneRenderer<Scene3D> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
			};

			struct Instance {
				mat4x4<float32> transform;
			};

		public:
			void build() override {
				_createRenderPass();

				depthStencilImage = createDepthStencilImage();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();

				vertexDescriptorSetLayout = createVertexDescriptorSetLayout();

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({vertexDescriptorSetLayout});

				vertexDescriptorSet = descriptorSets[0];

				cameraBuffer = createCameraBuffer();
				cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->getSize());
			}

			void postSetScene() override {
				createMaterialObjects();

				instanceBuffer = createInstanceBuffer();
				instanceMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->getSize());

				Entity entity = 0;

				while (entity < scene->getInstanceCount()) {
					updateTransform(entity);

					entity += 1;
				}

				view<ref<Geometry3D>> geometries = scene->getGeometries();

				for (const ref<Geometry3D> geometry : geometries) {
					vertexPositionComponentBufferSize += sizeof(float32) * geometry->getVertexPositionComponents().count();
					vertexPositionIndexBufferSize += sizeof(uint32) * geometry->getVertexPositionIndices().count();
					vertexNormalComponentBufferSize += sizeof(float32) * geometry->getVertexNormalComponents().count();
					vertexNormalIndexBufferSize += sizeof(uint32) * geometry->getVertexNormalIndices().count();
					vertexTextureCoordinateComponentBufferSize += sizeof(float32) * geometry->getVertexTextureCoordinateComponents().count();
					vertexTextureCoordinateIndexBufferSize += sizeof(uint32) * geometry->getVertexTextureCoordinateIndices().count();
				}

				const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

				vertexPositionComponentBufferSize = ceil(vertexPositionComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexPositionIndexBufferSize = ceil(vertexPositionIndexBufferSize, minStorageBufferOffsetAlignment);
				vertexNormalComponentBufferSize = ceil(vertexNormalComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexNormalIndexBufferSize = ceil(vertexNormalIndexBufferSize, minStorageBufferOffsetAlignment);
				vertexTextureCoordinateComponentBufferSize = ceil(vertexTextureCoordinateComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexTextureCoordinateIndexBufferSize = ceil(vertexTextureCoordinateIndexBufferSize, minStorageBufferOffsetAlignment);

				const ref<Buffer> vertexStagingBuffer = createStagingBuffer(
					vertexPositionComponentBufferSize +
					vertexPositionIndexBufferSize +
					vertexNormalComponentBufferSize +
					vertexNormalIndexBufferSize +
					vertexTextureCoordinateComponentBufferSize +
					vertexTextureCoordinateIndexBufferSize
				);

				uint8* mappedRange = vertexStagingBuffer->map<uint8>(0, vertexStagingBuffer->getSize());
				float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange);
				uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize);
				float32* vertexNormalComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize);
				uint32* vertexNormalIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize);
				float32* vertexTextureCoordinateComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize);
				uint32* vertexTextureCoordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize);

				for (const ref<Geometry3D> geometry : geometries) {
					const view<float32> vertexPositionComponents = geometry->getVertexPositionComponents();
					const view<uint32> vertexPositionIndices = geometry->getVertexPositionIndices();
					const view<float32> vertexNormalComponents = geometry->getVertexNormalComponents();
					const view<uint32> vertexNormalIndices = geometry->getVertexNormalIndices();
					const view<float32> vertexTextureCoordinateComponents = geometry->getVertexTextureCoordinateComponents();
					const view<uint32> vertexTextureCoordinateIndices = geometry->getVertexTextureCoordinateIndices();

					std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), vertexPositionComponentMappedRange);
					std::copy(vertexPositionIndices.begin(), vertexPositionIndices.end(), vertexPositionIndexMappedRange);
					std::copy(vertexNormalComponents.begin(), vertexNormalComponents.end(), vertexNormalComponentMappedRange);
					std::copy(vertexNormalIndices.begin(), vertexNormalIndices.end(), vertexNormalIndexMappedRange);
					std::copy(vertexTextureCoordinateComponents.begin(), vertexTextureCoordinateComponents.end(), vertexTextureCoordinateComponentMappedRange);
					std::copy(vertexTextureCoordinateIndices.begin(), vertexTextureCoordinateIndices.end(), vertexTextureCoordinateIndexMappedRange);

					vertexPositionComponentMappedRange += vertexPositionComponents.count();
					vertexPositionIndexMappedRange += vertexPositionIndices.count();
					vertexNormalComponentMappedRange += vertexNormalComponents.count();
					vertexNormalIndexMappedRange += vertexNormalIndices.count();
					vertexTextureCoordinateComponentMappedRange += vertexTextureCoordinateComponents.count();
					vertexTextureCoordinateIndexMappedRange += vertexTextureCoordinateIndices.count();
				}

				vertexStagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = vertexStagingBuffer->getSize();
				vertexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex");

				vertexBuffer = createBuffer(vertexBufferDescriptor);

				BufferCopyRegion vertexBufferCopyRegion{};
				vertexBufferCopyRegion.sourceOffset = 0;
				vertexBufferCopyRegion.destinationOffset = 0;
				vertexBufferCopyRegion.size = vertexBuffer->getSize();

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Write vertex buffer");

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(vertexStagingBuffer, vertexBuffer, {vertexBufferCopyRegion});

				queue->submit({commandEncoder->end()}, nullptr, nullptr, nullptr);
				queue->waitIdle();

				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				const ClearValue swapchainClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

				ClearValue depthStencilClearValue{};
				depthStencilClearValue.depth = 1.0f;
				depthStencilClearValue.stencil = 0;

				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {swapchainClearValue, depthStencilClearValue};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				// TODO
				// commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));

				const view<ref<Geometry3D>> geometries = scene->getGeometries();
				const view<Material> materials = scene->getMaterials();
				const view<MaterialReference> materialReferences = scene->getMaterialReferences();
				const view<RigidBody3D> instances = scene->getInstances();

				uint32 instanceIndex = 0;
				uint32 geometryIndex;
				uint32 indexOffset;

				while (instanceIndex < scene->getInstanceCount()) {
					const RigidBody3D& instance = instances[instanceIndex];
					const ref<Geometry3D> geometry = geometries[instance.geometryIndex];
					const MaterialReference& materialReference = materialReferences[instance.materialReferenceIndex];

					geometryIndex = 0;
					indexOffset = 0;

					while (geometryIndex < instance.geometryIndex) {
						const ref<Geometry3D> geometry = geometries[geometryIndex];

						indexOffset += static_cast<uint32>(geometry->getVertexPositionIndices().count());
						geometryIndex += 1;
					}

					commandEncoder->bindRenderPipeline(materialPipelines[materialReference.materialIndex]);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialReference.materialIndex], {vertexDescriptorSet/* , materialDescriptorSets[materialReference.materialIndex] */});
					commandEncoder->draw(static_cast<uint32>(geometry->getVertexPositionIndices().count()), 1, indexOffset, instanceIndex);

					instanceIndex += 1;
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				depthStencilImage = createDepthStencilImage();
				depthStencilImageView = createDepthStencilImageView();

				createFramebuffers();
				updateCamera();
			}

			void updateTransform(Entity entity) {
				instanceMappedRange[entity].transform = scene->getInstances()[entity].transform;
			}

			void updateCamera() {
				cameraMappedRange->projectionView = getCameraController<PerspectiveCamera>().getCamera().getProjectionView();
			}

		private:
			void _createRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
				swapchainAttachment.sampleCount = 1;
				swapchainAttachment.loadOperation = LoadOperation::CLEAR;
				swapchainAttachment.storeOperation = StoreOperation::UNDEFINED;
				swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				Attachment depthStencilAttachment{};
				depthStencilAttachment.format = ImageFormat::D32_SFLOAT;
				depthStencilAttachment.sampleCount = 1;
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
				renderPassDescriptor.attachments = {
					swapchainAttachment,
					depthStencilAttachment,
				};
				renderPassDescriptor.subpasses = {createSubpass(subpassDescriptor)};

				renderPass = createRenderPass(renderPassDescriptor);
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
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

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
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

			ref<ImageView> createDepthStencilImageView() const {
				ImageViewDescriptor depthStencilImageViewDescriptor{};
				depthStencilImageViewDescriptor.mipLevelOffset = 0;
				depthStencilImageViewDescriptor.mipLevelCount = 1;
				depthStencilImageViewDescriptor.layerOffset = 0;
				depthStencilImageViewDescriptor.layerCount = 1;

				SET_DEBUG_NAME(depthStencilImageViewDescriptor, "Depth stencil");

				return depthStencilImage->createView(depthStencilImageViewDescriptor);
			}

			ref<DescriptorSetLayout> createVertexDescriptorSetLayout() const {
				DescriptorSetLayoutBinding cameraBinding{};
				cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
				cameraBinding.descriptorCount = 1;
				cameraBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexPositionComponentBinding{};
				vertexPositionComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexPositionComponentBinding.descriptorCount = 1;
				vertexPositionComponentBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexPositionIndexBinding{};
				vertexPositionIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexPositionIndexBinding.descriptorCount = 1;
				vertexPositionIndexBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexNormalComponentBinding{};
				vertexNormalComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexNormalComponentBinding.descriptorCount = 1;
				vertexNormalComponentBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexNormalIndexBinding{};
				vertexNormalIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexNormalIndexBinding.descriptorCount = 1;
				vertexNormalIndexBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexTextureCoordinateComponentBinding{};
				vertexTextureCoordinateComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexTextureCoordinateComponentBinding.descriptorCount = 1;
				vertexTextureCoordinateComponentBinding.shaderStages = ShaderStage::VERTEX;

				DescriptorSetLayoutBinding vertexTextureCoordinateIndexBinding{};
				vertexTextureCoordinateIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexTextureCoordinateIndexBinding.descriptorCount = 1;
				vertexTextureCoordinateIndexBinding.shaderStages = ShaderStage::VERTEX;

				return createDescriptorSetLayout({
					cameraBinding,
					vertexPositionComponentBinding,
					vertexPositionIndexBinding,
					vertexNormalComponentBinding,
					vertexNormalIndexBinding,
					vertexTextureCoordinateComponentBinding,
					vertexTextureCoordinateIndexBinding,
				});
			}

			ref<Buffer> createInstanceBuffer() const {
				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = sizeof(Instance) * scene->getInstanceCount();
				instanceBufferDescriptor.usage = BufferUsage::VERTEX;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				instanceBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

				return createBuffer(instanceBufferDescriptor);
			}

			void createMaterialObjects() {
				const view<Material> materials = scene->getMaterials();

				materialDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materials.count());
				materialDescriptorSets = sequence<ref<DescriptorSet>>(materials.count());
				materialPipelineLayouts = sequence<ref<PipelineLayout>>(materials.count());
				materialPipelines = sequence<ref<RenderPipeline>>(materials.count());

				uint32 materialIndex = 0;

				PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<mat4x4<float32>>();

				ShaderModuleDescriptor materialVertexShaderModuleDescriptor{};
				materialVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				materialVertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(materialVertexShaderModuleDescriptor, "Material vertex");

				ShaderModuleDescriptor materialFragmentShaderModuleDescriptor{};
				materialFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				materialFragmentShaderModuleDescriptor.entryPoint = "main";

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor materialPipelineDescriptor{};
				materialPipelineDescriptor.vertexInputBindings = {instanceInputBinding};
				materialPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(materialVertexShaderModuleDescriptor, "Forward/Forward.vert");
				materialPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				materialPipelineDescriptor.polygonMode = PolygonMode::FILL;
				materialPipelineDescriptor.cullMode = CullMode::NONE;
				materialPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				materialPipelineDescriptor.lineWidth = 1.0f;
				materialPipelineDescriptor.minSampleShading = 0.0f;
				materialPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				materialPipelineDescriptor.depthStencilState.depthTest = true;
				materialPipelineDescriptor.depthStencilState.depthWrite = true;
				materialPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::LESS;
				materialPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				materialPipelineDescriptor.renderPass = renderPass;
				materialPipelineDescriptor.subpassIndex = 0;

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				while (materialIndex < materials.count()) {
					const Material& material = materials[materialIndex];

					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("Material [] fragment", materialIndex);

						SET_DEBUG_NAME(materialFragmentShaderModuleDescriptor, name);

						name = Logger::format("Material []", materialIndex);

						SET_DEBUG_NAME(materialPipelineLayoutDescriptor, name);
						SET_DEBUG_NAME(materialPipelineDescriptor, name);
					#endif

					materialDescriptorSetLayouts[materialIndex] = createDescriptorSetLayout(material.descriptorSetLayoutBindings);

					materialPipelineLayoutDescriptor.descriptorSetLayouts = {vertexDescriptorSetLayout/* , materialDescriptorSetLayouts[materialIndex] */};

					materialPipelineLayouts[materialIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

					materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialIndex];
					materialPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(materialFragmentShaderModuleDescriptor, material.shaderModulePath);

					materialPipelines[materialIndex] = createRenderPipeline(materialPipelineDescriptor);

					materialIndex += 1;
				}

				materialDescriptorSets = getDevice()->allocateDescriptorSets(materialDescriptorSetLayouts);
			}

			ref<Buffer> createCameraBuffer() const {
				BufferDescriptor cameraBufferDescriptor{};
				cameraBufferDescriptor.size = sizeof(Camera);
				cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
				cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				cameraBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(cameraBufferDescriptor, "Camera");

				return createBuffer(cameraBufferDescriptor);
			}

			void updateDescriptorSets() const {
				DescriptorBuffer cameraDescriptorBuffer{};
				cameraDescriptorBuffer.buffer = cameraBuffer;
				cameraDescriptorBuffer.offset = 0;
				cameraDescriptorBuffer.size = cameraBuffer->getSize();

				DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
				vertexPositionComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexPositionComponentDescriptorBuffer.offset = 0;
				vertexPositionComponentDescriptorBuffer.size = vertexPositionComponentBufferSize;

				DescriptorBuffer vertexPositionIndexDescriptorBuffer{};
				vertexPositionIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexPositionIndexDescriptorBuffer.offset = vertexPositionComponentBufferSize;
				vertexPositionIndexDescriptorBuffer.size = vertexPositionIndexBufferSize;

				DescriptorBuffer vertexNormalComponentDescriptorBuffer{};
				vertexNormalComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexNormalComponentDescriptorBuffer.offset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize;
				vertexNormalComponentDescriptorBuffer.size = vertexNormalComponentBufferSize;

				DescriptorBuffer vertexNormalIndexDescriptorBuffer{};
				vertexNormalIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexNormalIndexDescriptorBuffer.offset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize;
				vertexNormalIndexDescriptorBuffer.size = vertexNormalIndexBufferSize;

				DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer{};
				vertexTextureCoordinateComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexTextureCoordinateComponentDescriptorBuffer.offset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize;
				vertexTextureCoordinateComponentDescriptorBuffer.size = vertexTextureCoordinateComponentBufferSize;

				DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer{};
				vertexTextureCoordinateIndexDescriptorBuffer.buffer = vertexBuffer;
				vertexTextureCoordinateIndexDescriptorBuffer.offset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize;
				vertexTextureCoordinateIndexDescriptorBuffer.size = vertexTextureCoordinateIndexBufferSize;

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

				getDevice()->updateDescriptorSets({
					cameraWriteDescriptorSet,
					vertexPositionComponentWriteDescriptorSet,
					vertexPositionIndexWriteDescriptorSet,
					vertexNormalComponentWriteDescriptorSet,
					vertexNormalIndexWriteDescriptorSet,
					vertexTextureCoordinateComponentWriteDescriptorSet,
					vertexTextureCoordinateIndexWriteDescriptorSet,
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

			sequence<ref<RenderPipeline>> materialPipelines;

			ref<Buffer> cameraBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> vertexBuffer;

			ref<Image> depthStencilImage;

			ref<ImageView> depthStencilImageView;

			Camera* cameraMappedRange = nullptr;
			Instance* instanceMappedRange = nullptr;

			uint64 vertexPositionComponentBufferSize = 0;
			uint64 vertexPositionIndexBufferSize = 0;
			uint64 vertexNormalComponentBufferSize = 0;
			uint64 vertexNormalIndexBufferSize = 0;
			uint64 vertexTextureCoordinateComponentBufferSize = 0;
			uint64 vertexTextureCoordinateIndexBufferSize = 0;
	};
}