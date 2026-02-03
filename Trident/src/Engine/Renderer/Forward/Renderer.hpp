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
				createFramebuffers();
				createCameraBuffer();

				forwardDescriptorSetLayout = createForwardDescriptorSetLayout();
				forwardPipelineLayout = createForwardPipelineLayout();

				allocateDescriptorSets();

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<mat4x4<float32>>();

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(vertexShaderModuleDescriptor, "Forward vertex");

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(fragmentShaderModuleDescriptor, "Forward fragment");

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				RenderPipelineDescriptor forwardPipelineDescriptor{};
				forwardPipelineDescriptor.pipelineLayout = forwardPipelineLayout;
				forwardPipelineDescriptor.vertexInputBindings = {instanceInputBinding};
				forwardPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(vertexShaderModuleDescriptor, "Forward/Forward.vert");
				forwardPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(fragmentShaderModuleDescriptor, "Forward/Forward.frag");
				forwardPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
				forwardPipelineDescriptor.polygonMode = PolygonMode::FILL;
				forwardPipelineDescriptor.cullMode = CullMode::BACK;
				forwardPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
				forwardPipelineDescriptor.lineWidth = 1.0f;
				forwardPipelineDescriptor.minSampleShading = 0.0f;
				forwardPipelineDescriptor.colorBlendState.attachments = {colorBlendAttachment};
				forwardPipelineDescriptor.depthStencilState.depthTest = false;
				forwardPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
				forwardPipelineDescriptor.renderPass = renderPass;
				forwardPipelineDescriptor.subpassIndex = 0;

				SET_DEBUG_NAME(forwardPipelineDescriptor, "Forward");

				forwardPipeline = createRenderPipeline(forwardPipelineDescriptor);
			}

			void postSetScene() override {
				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = sizeof(Instance) * scene->getInstanceCount();
				instanceBufferDescriptor.usage = BufferUsage::VERTEX;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				instanceBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

				instanceBuffer = createBuffer(instanceBufferDescriptor);
				instanceMappedRange = instanceBuffer->map<Instance>(0, instanceBuffer->size);

				Entity entity = 0;

				while (entity < scene->getInstanceCount()) {
					updateTransform(entity);

					entity += 1;
				}

				uint64 vertexPositionComponentBufferSize = 0;
				uint64 vertexPositionIndexBufferSize = 0;
				uint64 vertexNormalComponentBufferSize = 0;
				uint64 vertexNormalIndexBufferSize = 0;
				uint64 vertexTextureCoordinateComponentBufferSize = 0;
				uint64 vertexTextureCoordinateIndexBufferSize = 0;
				uint32 geometryIndex = 0;

				while (geometryIndex < scene->getGeometryCount()) {
					const ref<Geometry3D> geometry = scene->getGeometry(geometryIndex);

					vertexPositionComponentBufferSize += sizeof(float32) * geometry->getVertexPositionComponents().count();
					vertexPositionIndexBufferSize += sizeof(uint32) * geometry->getVertexPositionIndices().count();
					vertexNormalComponentBufferSize += sizeof(float32) * geometry->getVertexNormalComponents().count();
					vertexNormalIndexBufferSize += sizeof(uint32) * geometry->getVertexNormalIndices().count();
					vertexTextureCoordinateComponentBufferSize += sizeof(float32) * geometry->getVertexTextureCoordinateComponents().count();
					vertexTextureCoordinateIndexBufferSize += sizeof(uint32) * geometry->getVertexTextureCoordinateIndices().count();

					geometryIndex += 1;
				}

				const ref<Buffer> stagingBuffer = createStagingBuffer(
					vertexPositionComponentBufferSize +
					vertexPositionIndexBufferSize +
					vertexNormalComponentBufferSize +
					vertexNormalIndexBufferSize +
					vertexTextureCoordinateComponentBufferSize +
					vertexTextureCoordinateIndexBufferSize
				);

				uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
				float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange);
				uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize);
				float32* vertexNormalComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize);
				uint32* vertexNormalIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize);
				float32* vertexTextureCoordinateComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize);
				uint32* vertexTextureCoordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize);

				geometryIndex = 0;

				while (geometryIndex < scene->getGeometryCount()) {
					const ref<Geometry3D> geometry = scene->getGeometry(geometryIndex);
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
					geometryIndex += 1;
				}

				stagingBuffer->unmap();

				BufferDescriptor vertexPositionComponentBufferDescriptor{};
				vertexPositionComponentBufferDescriptor.size = vertexPositionComponentBufferSize;
				vertexPositionComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionComponentBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexPositionComponentBufferDescriptor, "Vertex position component");

				vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);

				BufferDescriptor vertexPositionIndexBufferDescriptor{};
				vertexPositionIndexBufferDescriptor.size = vertexPositionIndexBufferSize;
				vertexPositionIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexPositionIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionIndexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexPositionIndexBufferDescriptor, "Vertex position index");

				vertexPositionIndexBuffer = createBuffer(vertexPositionIndexBufferDescriptor);

				BufferDescriptor vertexNormalComponentBufferDescriptor{};
				vertexNormalComponentBufferDescriptor.size = vertexNormalComponentBufferSize;
				vertexNormalComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexNormalComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexNormalComponentBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexNormalComponentBufferDescriptor, "Vertex normal component");

				vertexNormalComponentBuffer = createBuffer(vertexNormalComponentBufferDescriptor);

				BufferDescriptor vertexNormalIndexBufferDescriptor{};
				vertexNormalIndexBufferDescriptor.size = vertexNormalIndexBufferSize;
				vertexNormalIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexNormalIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexNormalIndexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexNormalIndexBufferDescriptor, "Vertex normal index");

				vertexNormalIndexBuffer = createBuffer(vertexNormalIndexBufferDescriptor);

				BufferDescriptor vertexTextureCoordinateComponentBufferDescriptor{};
				vertexTextureCoordinateComponentBufferDescriptor.size = vertexTextureCoordinateComponentBufferSize;
				vertexTextureCoordinateComponentBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexTextureCoordinateComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexTextureCoordinateComponentBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexTextureCoordinateComponentBufferDescriptor, "Vertex texture coordinate component");

				vertexTextureCoordinateComponentBuffer = createBuffer(vertexTextureCoordinateComponentBufferDescriptor);

				BufferDescriptor vertexTextureCoordinateIndexBufferDescriptor{};
				vertexTextureCoordinateIndexBufferDescriptor.size = vertexTextureCoordinateIndexBufferSize;
				vertexTextureCoordinateIndexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexTextureCoordinateIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexTextureCoordinateIndexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexTextureCoordinateIndexBufferDescriptor, "Vertex texture coordinate index");

				vertexTextureCoordinateIndexBuffer = createBuffer(vertexTextureCoordinateIndexBufferDescriptor);

				BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
				vertexPositionComponentBufferCopyRegion.sourceOffset = 0;
				vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
				vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBufferSize;

				BufferCopyRegion vertexPositionIndexBufferCopyRegion{};
				vertexPositionIndexBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize;
				vertexPositionIndexBufferCopyRegion.destinationOffset = 0;
				vertexPositionIndexBufferCopyRegion.size = vertexPositionIndexBufferSize;

				BufferCopyRegion vertexNormalComponentBufferCopyRegion{};
				vertexNormalComponentBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize;
				vertexNormalComponentBufferCopyRegion.destinationOffset = 0;
				vertexNormalComponentBufferCopyRegion.size = vertexNormalComponentBufferSize;

				BufferCopyRegion vertexNormalIndexBufferCopyRegion{};
				vertexNormalIndexBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize;
				vertexNormalIndexBufferCopyRegion.destinationOffset = 0;
				vertexNormalIndexBufferCopyRegion.size = vertexNormalIndexBufferSize;

				BufferCopyRegion vertexTextureCoordinateComponentBufferCopyRegion{};
				vertexTextureCoordinateComponentBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize;
				vertexTextureCoordinateComponentBufferCopyRegion.destinationOffset = 0;
				vertexTextureCoordinateComponentBufferCopyRegion.size = vertexTextureCoordinateComponentBufferSize;

				BufferCopyRegion vertexTextureCoordinateIndexBufferCopyRegion{};
				vertexTextureCoordinateIndexBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize;
				vertexTextureCoordinateIndexBufferCopyRegion.destinationOffset = 0;
				vertexTextureCoordinateIndexBufferCopyRegion.size = vertexTextureCoordinateIndexBufferSize;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Copy buffers");

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionIndexBuffer, {vertexPositionIndexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalComponentBuffer, {vertexNormalComponentBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexNormalIndexBuffer, {vertexNormalIndexBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateComponentBuffer, {vertexTextureCoordinateComponentBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexTextureCoordinateIndexBuffer, {vertexTextureCoordinateIndexBufferCopyRegion});

				queue->submit({commandEncoder->end()}, nullptr, nullptr, nullptr);
				queue->waitIdle();

				updateDescriptorSets();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {
				const Extent2D swapchainExtent = getSwapchain()->getExtent();
				const float32 width = static_cast<float32>(swapchainExtent.width);
				const float32 height = static_cast<float32>(swapchainExtent.height);

				BeginRenderPassDescriptor beginRenderPassDescriptor{};
				beginRenderPassDescriptor.framebuffer = framebuffers[getImageIndex()];
				beginRenderPassDescriptor.clearValues = {{0.0f, 0.0f, 0.0f, 1.0f}};

				commandEncoder->beginRenderPass(beginRenderPassDescriptor);
				commandEncoder->bindRenderPipeline(forwardPipeline);
				commandEncoder->bindDescriptorSets(forwardPipelineLayout, {forwardDescriptorSet});
				commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
				commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
				commandEncoder->setScissor(0, 0, swapchainExtent);

				const view<RigidBody3D> rigidBodyPool = scene->getInstances();
				const view<ref<Geometry3D>> geometryPool = scene->getGeometries();

				uint32 instanceIndex = 0;
				uint32 geometryIndex;
				uint32 indexOffset;

				while (instanceIndex < scene->getInstanceCount()) {
					const RigidBody3D& rigidBody = rigidBodyPool[instanceIndex];
					const ref<Geometry3D> geometry = geometryPool[rigidBody.geometryIndex];

					geometryIndex = 0;
					indexOffset = 0;

					while (geometryIndex < rigidBody.geometryIndex) {
						const ref<Geometry3D> geometry = geometryPool[geometryIndex];

						indexOffset += static_cast<uint32>(geometry->getVertexPositionIndices().count());
						geometryIndex += 1;
					}

					commandEncoder->draw(static_cast<uint32>(geometry->getVertexPositionIndices().count()), 1, indexOffset, instanceIndex);

					instanceIndex += 1;
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
				updateCamera();
			}

			void updateTransform(Entity entity) {
				instanceMappedRange[entity].transform = scene->getInstance(entity).transform;
			}

			void updateCamera() {
				cameraMappedRange->projectionView = getCameraController<PerspectiveCamera>().getCamera().getProjectionView();
			}

			void postSetCameraController() override {
				updateCamera();
			}

		private:
			void _createRenderPass() {
				Attachment swapchainAttachment{};
				swapchainAttachment.format = getSurface()->getFormat();
				swapchainAttachment.sampleCount = 1;
				swapchainAttachment.loadOperation = LoadOperation::CLEAR;
				swapchainAttachment.storeOperation = StoreOperation::STORE;
				swapchainAttachment.finalLayout = ImageLayout::PRESENT_SRC;

				AttachmentReference colorAttachmentReference{};
				colorAttachmentReference.index = 0;
				colorAttachmentReference.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				SubpassDescriptor subpassDescriptor{};
				subpassDescriptor.colorAttachmentReferences = {colorAttachmentReference};

				RenderPassDescriptor renderPassDescriptor{};
				renderPassDescriptor.attachments = {swapchainAttachment};
				renderPassDescriptor.subpasses = {createSubpass(subpassDescriptor)};

				renderPass = createRenderPass(renderPassDescriptor);
			}

			void createFramebuffers() {
				const ref<Swapchain> swapchain = getSwapchain();

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = {nullptr};
				framebufferDescriptor.extent = swapchain->getExtent();

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 framebufferIndex = 0;

				while (framebufferIndex < framebuffers.count()) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight framebuffer []", framebufferIndex);

						framebufferDescriptor.name = name;
					#endif

					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

					framebufferIndex += 1;
				}
			}

			void createCameraBuffer() {
				BufferDescriptor cameraBufferDescriptor{};
				cameraBufferDescriptor.size = sizeof(Camera);
				cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
				cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				cameraBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(cameraBufferDescriptor, "Camera");

				cameraBuffer = createBuffer(cameraBufferDescriptor);
				cameraMappedRange = cameraBuffer->map<Camera>(0, cameraBuffer->size);
			}

			ref<DescriptorSetLayout> createForwardDescriptorSetLayout() const {
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

			ref<PipelineLayout> createForwardPipelineLayout() const {
				PipelineLayoutDescriptor forwardPipelineLayoutDescriptor{};
				forwardPipelineLayoutDescriptor.descriptorSetLayouts = {forwardDescriptorSetLayout};

				SET_DEBUG_NAME(forwardPipelineLayoutDescriptor, "Forward");

				return createPipelineLayout(forwardPipelineLayoutDescriptor);
			}

			void allocateDescriptorSets() {
				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({forwardDescriptorSetLayout});

				forwardDescriptorSet = descriptorSets[0];
			}

			void updateDescriptorSets() const {
				DescriptorBuffer cameraDescriptorBuffer{};
				cameraDescriptorBuffer.buffer = cameraBuffer;
				cameraDescriptorBuffer.offset = 0;
				cameraDescriptorBuffer.size = cameraBuffer->size;

				DescriptorBuffer vertexPositionComponentDescriptorBuffer{};
				vertexPositionComponentDescriptorBuffer.buffer = vertexPositionComponentBuffer;
				vertexPositionComponentDescriptorBuffer.offset = 0;
				vertexPositionComponentDescriptorBuffer.size = vertexPositionComponentBuffer->size;

				DescriptorBuffer vertexPositionIndexDescriptorBuffer{};
				vertexPositionIndexDescriptorBuffer.buffer = vertexPositionIndexBuffer;
				vertexPositionIndexDescriptorBuffer.offset = 0;
				vertexPositionIndexDescriptorBuffer.size = vertexPositionIndexBuffer->size;

				DescriptorBuffer vertexNormalComponentDescriptorBuffer{};
				vertexNormalComponentDescriptorBuffer.buffer = vertexNormalComponentBuffer;
				vertexNormalComponentDescriptorBuffer.offset = 0;
				vertexNormalComponentDescriptorBuffer.size = vertexNormalComponentBuffer->size;

				DescriptorBuffer vertexNormalIndexDescriptorBuffer{};
				vertexNormalIndexDescriptorBuffer.buffer = vertexNormalIndexBuffer;
				vertexNormalIndexDescriptorBuffer.offset = 0;
				vertexNormalIndexDescriptorBuffer.size = vertexNormalIndexBuffer->size;

				DescriptorBuffer vertexTextureCoordinateComponentDescriptorBuffer{};
				vertexTextureCoordinateComponentDescriptorBuffer.buffer = vertexTextureCoordinateComponentBuffer;
				vertexTextureCoordinateComponentDescriptorBuffer.offset = 0;
				vertexTextureCoordinateComponentDescriptorBuffer.size = vertexTextureCoordinateComponentBuffer->size;

				DescriptorBuffer vertexTextureCoordinateIndexDescriptorBuffer{};
				vertexTextureCoordinateIndexDescriptorBuffer.buffer = vertexTextureCoordinateIndexBuffer;
				vertexTextureCoordinateIndexDescriptorBuffer.offset = 0;
				vertexTextureCoordinateIndexDescriptorBuffer.size = vertexTextureCoordinateIndexBuffer->size;

				WriteDescriptorSet cameraWriteDescriptorSet{};
				cameraWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				cameraWriteDescriptorSet.bindingIndex = 0;
				cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

				WriteDescriptorSet vertexPositionComponentWriteDescriptorSet{};
				vertexPositionComponentWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				vertexPositionComponentWriteDescriptorSet.bindingIndex = 1;
				vertexPositionComponentWriteDescriptorSet.descriptorBuffers = {vertexPositionComponentDescriptorBuffer};

				WriteDescriptorSet vertexPositionIndexWriteDescriptorSet{};
				vertexPositionIndexWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				vertexPositionIndexWriteDescriptorSet.bindingIndex = 2;
				vertexPositionIndexWriteDescriptorSet.descriptorBuffers = {vertexPositionIndexDescriptorBuffer};

				WriteDescriptorSet vertexNormalComponentWriteDescriptorSet{};
				vertexNormalComponentWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				vertexNormalComponentWriteDescriptorSet.bindingIndex = 3;
				vertexNormalComponentWriteDescriptorSet.descriptorBuffers = {vertexNormalComponentDescriptorBuffer};

				WriteDescriptorSet vertexNormalIndexWriteDescriptorSet{};
				vertexNormalIndexWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				vertexNormalIndexWriteDescriptorSet.bindingIndex = 4;
				vertexNormalIndexWriteDescriptorSet.descriptorBuffers = {vertexNormalIndexDescriptorBuffer};

				WriteDescriptorSet vertexTextureCoordinateComponentWriteDescriptorSet{};
				vertexTextureCoordinateComponentWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
				vertexTextureCoordinateComponentWriteDescriptorSet.bindingIndex = 5;
				vertexTextureCoordinateComponentWriteDescriptorSet.descriptorBuffers = {vertexTextureCoordinateComponentDescriptorBuffer};

				WriteDescriptorSet vertexTextureCoordinateIndexWriteDescriptorSet{};
				vertexTextureCoordinateIndexWriteDescriptorSet.descriptorSet = forwardDescriptorSet;
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

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> forwardPipelineLayout;

			ref<RenderPipeline> forwardPipeline;

			ref<DescriptorSetLayout> forwardDescriptorSetLayout;

			ref<DescriptorSet> forwardDescriptorSet;

			ref<Buffer> cameraBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexPositionIndexBuffer;
			ref<Buffer> vertexNormalComponentBuffer;
			ref<Buffer> vertexNormalIndexBuffer;
			ref<Buffer> vertexTextureCoordinateComponentBuffer;
			ref<Buffer> vertexTextureCoordinateIndexBuffer;

			Camera* cameraMappedRange = nullptr;
			Instance* instanceMappedRange = nullptr;
	};
}