// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class ForwardRenderer : public SceneRenderer<Scene3D> {
		private:
			struct Transform {
				mat4x4<float32> transform;
			};

			struct Camera {
				mat4x4<float32> projectionView;
			};

		public:
			void build() override {
				_createRenderPass();
				createFramebuffers();
				createCameraBuffer();

				DescriptorSetLayoutBinding cameraBinding{};
				cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
				cameraBinding.descriptorCount = 1;
				cameraBinding.shaderStages = ShaderStage::VERTEX;

				cameraDescriptorSetLayout = createDescriptorSetLayout({cameraBinding});

				const sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets({cameraDescriptorSetLayout});

				cameraDescriptorSet = descriptorSets[0];

				DescriptorBuffer cameraDescriptorBuffer{};
				cameraDescriptorBuffer.buffer = cameraBuffer;
				cameraDescriptorBuffer.offset = 0;
				cameraDescriptorBuffer.size = cameraBuffer->size;

				WriteDescriptorSet cameraWriteDescriptorSet{};
				cameraWriteDescriptorSet.descriptorSet = cameraDescriptorSet;
				cameraWriteDescriptorSet.bindingIndex = 0;
				cameraWriteDescriptorSet.descriptorBuffers = {cameraDescriptorBuffer};

				getDevice()->updateDescriptorSets({cameraWriteDescriptorSet});

				PipelineLayoutDescriptor forwardPipelineLayoutDescriptor{};
				forwardPipelineLayoutDescriptor.descriptorSetLayouts = {cameraDescriptorSetLayout};

				SET_DEBUG_NAME(forwardPipelineLayoutDescriptor, "Forward");

				forwardPipelineLayout = createPipelineLayout(forwardPipelineLayoutDescriptor);

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<mat4x4<float32>>();

				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<svec3<float32>>();

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
				forwardPipelineDescriptor.vertexInputBindings = {instanceInputBinding, vertexInputBinding};
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
				const uint64 transformBufferSize = sizeof(Transform) * scene->getInstanceCount();

				BufferDescriptor transformBufferDescriptor{};
				transformBufferDescriptor.size = transformBufferSize;
				transformBufferDescriptor.usage = BufferUsage::VERTEX;
				transformBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				transformBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(transformBufferDescriptor, "Transform");

				transformBuffer = createBuffer(transformBufferDescriptor);
				transformMappedRange = transformBuffer->map<Transform>(0, transformBuffer->size);

				Entity entity = 0;

				while (entity < scene->getInstanceCount()) {
					updateTransform(entity);

					entity += 1;
				}

				uint64 vertexPositionComponentBufferSize = 0;
				uint64 vertexPositionIndexBufferSize = 0;
				uint32 geometryIndex = 0;

				while (geometryIndex < scene->getGeometryCount()) {
					const ref<Geometry3D> geometry = scene->getGeometry(geometryIndex);

					vertexPositionComponentBufferSize += sizeof(float32) * geometry->getVertexPositionComponents().count();
					vertexPositionIndexBufferSize += sizeof(uint32) * geometry->getVertexPositionIndices().count();

					geometryIndex += 1;
				}

				const ref<Buffer> stagingBuffer = createStagingBuffer(vertexPositionComponentBufferSize + vertexPositionIndexBufferSize);

				uint8* mappedRange = stagingBuffer->map<uint8>(0, stagingBuffer->size);
				float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange);
				uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize);
				uint64 vertexPositionComponentOffset = 0;
				uint64 vertexPositionIndexOffset = 0;

				geometryIndex = 0;

				while (geometryIndex < scene->getGeometryCount()) {
					const ref<Geometry3D> geometry = scene->getGeometry(geometryIndex);
					const view<float32> vertexPositionComponents = geometry->getVertexPositionComponents();
					const view<uint32> vertexPositionIndices = geometry->getVertexPositionIndices();

					std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), &vertexPositionComponentMappedRange[vertexPositionComponentOffset]);
					std::copy(vertexPositionIndices.begin(), vertexPositionIndices.end(), &vertexPositionIndexMappedRange[vertexPositionIndexOffset]);

					vertexPositionComponentOffset += vertexPositionComponents.count();
					vertexPositionIndexOffset += vertexPositionIndices.count();
					geometryIndex += 1;
				}

				stagingBuffer->unmap();

				BufferDescriptor vertexPositionComponentBufferDescriptor{};
				vertexPositionComponentBufferDescriptor.size = vertexPositionComponentBufferSize;
				vertexPositionComponentBufferDescriptor.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
				vertexPositionComponentBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionComponentBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexPositionComponentBufferDescriptor, "Vertex position component");

				vertexPositionComponentBuffer = createBuffer(vertexPositionComponentBufferDescriptor);

				BufferDescriptor vertexPositionIndexBufferDescriptor{};
				vertexPositionIndexBufferDescriptor.size = vertexPositionIndexBufferSize;
				vertexPositionIndexBufferDescriptor.usage = BufferUsage::INDEX | BufferUsage::TRANSFER_DST;
				vertexPositionIndexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexPositionIndexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexPositionIndexBufferDescriptor, "Vertex position index");

				vertexPositionIndexBuffer = createBuffer(vertexPositionIndexBufferDescriptor);

				BufferCopyRegion vertexPositionComponentBufferCopyRegion{};
				vertexPositionComponentBufferCopyRegion.sourceOffset = 0;
				vertexPositionComponentBufferCopyRegion.destinationOffset = 0;
				vertexPositionComponentBufferCopyRegion.size = vertexPositionComponentBufferSize;

				BufferCopyRegion vertexPositionIndexBufferCopyRegion{};
				vertexPositionIndexBufferCopyRegion.sourceOffset = vertexPositionComponentBufferSize;
				vertexPositionIndexBufferCopyRegion.destinationOffset = 0;
				vertexPositionIndexBufferCopyRegion.size = vertexPositionIndexBufferSize;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Copy buffers");

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionComponentBuffer, {vertexPositionComponentBufferCopyRegion});
				commandEncoder->copyBufferToBuffer(stagingBuffer, vertexPositionIndexBuffer, {vertexPositionIndexBufferCopyRegion});

				queue->submit({commandEncoder->end()}, nullptr, nullptr, nullptr);
				queue->waitIdle();
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
				commandEncoder->bindDescriptorSets(forwardPipelineLayout, {cameraDescriptorSet});
				commandEncoder->bindVertexBuffer(0, transformBuffer, 0, sizeof(Transform));
				commandEncoder->bindVertexBuffer(1, vertexPositionComponentBuffer, 0, sizeof(svec3<float32>));
				commandEncoder->bindIndexBuffer<uint32>(vertexPositionIndexBuffer, 0);
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

					commandEncoder->drawIndexed(static_cast<uint32>(geometry->getVertexPositionIndices().count()), 1, indexOffset, instanceIndex, 0);

					instanceIndex += 1;
				}

				commandEncoder->endRenderPass();
			}

			void resize(WindowResizeEvent& event) override {
				createFramebuffers();
				updateCamera();
			}

			void updateTransform(Entity entity) {
				transformMappedRange[entity].transform = scene->getInstance(entity).transform;
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

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<PipelineLayout> forwardPipelineLayout;

			ref<RenderPipeline> forwardPipeline;

			ref<DescriptorSetLayout> cameraDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;

			ref<Buffer> cameraBuffer;
			ref<Buffer> transformBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexPositionIndexBuffer;
			ref<Buffer> vertexNormalBuffer;
			ref<Buffer> vertexNormalIndexBuffer;
			ref<Buffer> vertexTextureCoordinateBuffer;
			ref<Buffer> vertexTextureCoordinateIndexBuffer;

			Camera* cameraMappedRange = nullptr;
			Transform* transformMappedRange = nullptr;
	};
}