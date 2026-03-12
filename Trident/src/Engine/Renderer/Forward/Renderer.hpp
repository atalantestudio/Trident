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

				view<ref<Geometry3D>> geometries = scene->getGeometries();
				uint64 vertexPositionComponentBufferSize = 0;
				uint64 vertexPositionIndexBufferSize = 0;
				uint64 vertexNormalComponentBufferSize = 0;
				uint64 vertexNormalIndexBufferSize = 0;
				uint64 vertexTextureCoordinateComponentBufferSize = 0;
				uint64 vertexTextureCoordinateIndexBufferSize = 0;
				uint64 vertexTangentComponentBufferSize = 0;

				for (const ref<Geometry3D> geometry : geometries) {
					vertexPositionComponentBufferSize += sizeof(float32) * geometry->getVertexPositionComponents().count();
					vertexPositionIndexBufferSize += sizeof(uint32) * geometry->getVertexPositionIndices().count();
					vertexNormalComponentBufferSize += sizeof(float32) * geometry->getVertexNormalComponents().count();
					vertexNormalIndexBufferSize += sizeof(uint32) * geometry->getVertexNormalIndices().count();
					vertexTextureCoordinateComponentBufferSize += sizeof(float32) * geometry->getVertexTextureCoordinateComponents().count();
					vertexTextureCoordinateIndexBufferSize += sizeof(uint32) * geometry->getVertexTextureCoordinateIndices().count();
					vertexTangentComponentBufferSize += sizeof(float32) * geometry->getVertexTangentComponents().count();
				}

				const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

				vertexPositionComponentBufferSize = ceil(vertexPositionComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexPositionIndexBufferSize = ceil(vertexPositionIndexBufferSize, minStorageBufferOffsetAlignment);
				vertexNormalComponentBufferSize = ceil(vertexNormalComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexNormalIndexBufferSize = ceil(vertexNormalIndexBufferSize, minStorageBufferOffsetAlignment);
				vertexTextureCoordinateComponentBufferSize = ceil(vertexTextureCoordinateComponentBufferSize, minStorageBufferOffsetAlignment);
				vertexTextureCoordinateIndexBufferSize = ceil(vertexTextureCoordinateIndexBufferSize, minStorageBufferOffsetAlignment);
				vertexTangentComponentBufferSize = ceil(vertexTangentComponentBufferSize, minStorageBufferOffsetAlignment);

				const ref<Buffer<uint8>> vertexStagingBuffer = createStagingBuffer<uint8>(vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize + vertexTextureCoordinateIndexBufferSize + vertexTangentComponentBufferSize);

				uint8* mappedRange = vertexStagingBuffer->map(0, vertexStagingBuffer->getSize());
				float32* vertexPositionComponentMappedRange = pointer_cast<float32*>(mappedRange);
				uint32* vertexPositionIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize);
				float32* vertexNormalComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize);
				uint32* vertexNormalIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize);
				float32* vertexTextureCoordinateComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize);
				uint32* vertexTextureCoordinateIndexMappedRange = pointer_cast<uint32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize);
				float32* vertexTangentComponentMappedRange = pointer_cast<float32*>(mappedRange + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize + vertexTextureCoordinateIndexBufferSize);

				for (const ref<Geometry3D> geometry : geometries) {
					const view<float32> vertexPositionComponents = geometry->getVertexPositionComponents();
					const view<uint32> vertexPositionIndices = geometry->getVertexPositionIndices();
					const view<float32> vertexNormalComponents = geometry->getVertexNormalComponents();
					const view<uint32> vertexNormalIndices = geometry->getVertexNormalIndices();
					const view<float32> vertexTextureCoordinateComponents = geometry->getVertexTextureCoordinateComponents();
					const view<uint32> vertexTextureCoordinateIndices = geometry->getVertexTextureCoordinateIndices();
					const view<float32> vertexTangentComponents = geometry->getVertexTangentComponents();

					std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), vertexPositionComponentMappedRange);
					std::copy(vertexPositionIndices.begin(), vertexPositionIndices.end(), vertexPositionIndexMappedRange);
					std::copy(vertexNormalComponents.begin(), vertexNormalComponents.end(), vertexNormalComponentMappedRange);
					std::copy(vertexNormalIndices.begin(), vertexNormalIndices.end(), vertexNormalIndexMappedRange);
					std::copy(vertexTextureCoordinateComponents.begin(), vertexTextureCoordinateComponents.end(), vertexTextureCoordinateComponentMappedRange);
					std::copy(vertexTextureCoordinateIndices.begin(), vertexTextureCoordinateIndices.end(), vertexTextureCoordinateIndexMappedRange);
					std::copy(vertexTangentComponents.begin(), vertexTangentComponents.end(), vertexTangentComponentMappedRange);

					vertexPositionComponentMappedRange += vertexPositionComponents.count();
					vertexPositionIndexMappedRange += vertexPositionIndices.count();
					vertexNormalComponentMappedRange += vertexNormalComponents.count();
					vertexNormalIndexMappedRange += vertexNormalIndices.count();
					vertexTextureCoordinateComponentMappedRange += vertexTextureCoordinateComponents.count();
					vertexTextureCoordinateIndexMappedRange += vertexTextureCoordinateIndices.count();
					vertexTangentComponentMappedRange += vertexTangentComponents.count();
				}

				vertexStagingBuffer->unmap();

				BufferDescriptor vertexBufferDescriptor{};
				vertexBufferDescriptor.size = vertexStagingBuffer->getSize();
				vertexBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST;
				vertexBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				vertexBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(vertexBufferDescriptor, "Vertex");

				vertexBuffer = createBuffer<uint8>(vertexBufferDescriptor);

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

				queue->submit({commandEncoder->end()});
				queue->waitIdle();

				updateDescriptorSets(vertexPositionComponentBufferSize, vertexPositionIndexBufferSize, vertexNormalComponentBufferSize, vertexNormalIndexBufferSize, vertexTextureCoordinateComponentBufferSize, vertexTextureCoordinateIndexBufferSize, vertexTangentComponentBufferSize);
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
				// commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));

				const view<ref<Geometry3D>> geometries = scene->getGeometries();
				const view<Material> materials = scene->getMaterials();
				const view<MaterialReference> materialReferences = scene->getFlatMaterialReferences();
				const view<RigidBody3D> instances = scene->getInstances();

				uint32 instanceIndex = 0;

				while (instanceIndex < instances.count()) {
					const RigidBody3D& instance = instances[instanceIndex];
					const ref<Geometry3D> geometry = geometries[instance.geometryIndex];
					const MaterialReference& materialReference = materialReferences[instance.materialReferenceIndex];
					const uint32 vertexCount = static_cast<uint32>(geometry->getVertexPositionIndices().count());
					const uint32 indexOffset = scene->getVertexPositionIndexOffset(instance.geometryIndex);

					commandEncoder->bindGraphicsPipeline(materialPipelines[materialReference.materialIndex]);
					commandEncoder->bindVertexBuffer(0, instanceBuffer, 0, sizeof(Instance));
					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialReference.materialIndex], 0, {vertexDescriptorSet, materialDescriptorSets[instance.materialReferenceIndex]});
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
				ImageViewDescriptor colorImageView{};
				colorImageView.mipLevelOffset = 0;
				colorImageView.mipLevelCount = 1;
				colorImageView.layerOffset = 0;
				colorImageView.layerCount = 1;

				SET_DEBUG_NAME(colorImageView, "Color (multisampled)");

				return colorImage->createView(colorImageView);
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
				cameraBinding.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT;

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

				DescriptorSetLayoutBinding vertexTangentComponentBinding{};
				vertexTangentComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				vertexTangentComponentBinding.descriptorCount = 1;
				vertexTangentComponentBinding.shaderStages = ShaderStage::VERTEX;

				return createDescriptorSetLayout({
					cameraBinding,
					vertexPositionComponentBinding,
					vertexPositionIndexBinding,
					vertexNormalComponentBinding,
					vertexNormalIndexBinding,
					vertexTextureCoordinateComponentBinding,
					vertexTextureCoordinateIndexBinding,
					vertexTangentComponentBinding,
				});
			}

			ref<Buffer<Instance>> createInstanceBuffer() const {
				BufferDescriptor instanceBufferDescriptor{};
				instanceBufferDescriptor.size = sizeof(Instance) * scene->getInstanceCount();
				instanceBufferDescriptor.usage = BufferUsage::VERTEX;
				instanceBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				instanceBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(instanceBufferDescriptor, "Instance");

				return createBuffer<Instance>(instanceBufferDescriptor);
			}

			void createMaterialObjects() {
				// TODO: Use a larger extent, e.g. 4096 * 4096 * 1.
				static constexpr Extent3D IMAGE_EXTENT = {1024, 1024, 1};

				const view<Material> materials = scene->getMaterials();

				// Each material has N bindings. 1 image asset is specified per binding for each material reference.
				sequence<sequence<sequence<ref<ImageAsset>>>> materialImageAssets(materials.count());

				materialDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materials.count());
				materialDescriptorSets = sequence<ref<DescriptorSet>>(materials.count());
				materialPipelineLayouts = sequence<ref<PipelineLayout>>(materials.count());
				materialPipelines = sequence<ref<GraphicsPipeline>>(materials.count());
				materialImages = sequence<sequence<ref<Image>>>(materials.count());

				PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

				VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
				instanceInputBinding.registerAttribute<mat4x4<float32>>();
				instanceInputBinding.registerAttribute<mat3x3<float32>>();

				ShaderModuleDescriptor materialVertexShaderModuleDescriptor{};
				materialVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				materialVertexShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(materialVertexShaderModuleDescriptor, "Material vertex");

				ShaderModuleDescriptor materialFragmentShaderModuleDescriptor{};
				materialFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				materialFragmentShaderModuleDescriptor.entryPoint = "main";

				ColorBlendAttachment colorBlendAttachment{};
				colorBlendAttachment.blend = false;

				GraphicsPipelineDescriptor materialPipelineDescriptor{};
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

				ImageDescriptor materialDescriptorSetLayoutBindingImageDescriptor{};
				materialDescriptorSetLayoutBindingImageDescriptor.type = ImageType::TEXTURE_2D;
				materialDescriptorSetLayoutBindingImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				materialDescriptorSetLayoutBindingImageDescriptor.extent = IMAGE_EXTENT;
				materialDescriptorSetLayoutBindingImageDescriptor.mipLevelCount = 1;
				materialDescriptorSetLayoutBindingImageDescriptor.sampleCount = 1;
				materialDescriptorSetLayoutBindingImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
				materialDescriptorSetLayoutBindingImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				uint32 materialIndex = 0;
				uint32 bindingIndex = 0;
				uint32 materialBindingCount = 0;
				uint32 materialReferenceIndex = 0;
				uint32 imageCopyRegionCount = 0;
				uint64 imageStagingBufferSize = 0;

				while (materialIndex < materials.count()) {
					const Material& material = materials[materialIndex];
					const view<MaterialReference> materialReferences = scene->getMaterialReferences(materialIndex);

					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("Material [] fragment", materialIndex);

						SET_DEBUG_NAME(materialFragmentShaderModuleDescriptor, name);

						name = Logger::format("Material []", materialIndex);

						SET_DEBUG_NAME(materialPipelineLayoutDescriptor, name);
						SET_DEBUG_NAME(materialPipelineDescriptor, name);
					#endif

					materialDescriptorSetLayouts[materialIndex] = createDescriptorSetLayout(material.descriptorSetLayoutBindings);

					materialPipelineLayoutDescriptor.descriptorSetLayouts = {vertexDescriptorSetLayout, materialDescriptorSetLayouts[materialIndex]};

					materialPipelineLayouts[materialIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

					materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialIndex];
					materialPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(materialFragmentShaderModuleDescriptor, material.shaderModulePath);

					materialPipelines[materialIndex] = createGraphicsPipeline(materialPipelineDescriptor);

					materialImageAssets[materialIndex] = sequence<sequence<ref<ImageAsset>>>(material.descriptorSetLayoutBindings.count());
					materialImages[materialIndex] = sequence<ref<Image>>(material.descriptorSetLayoutBindings.count());

					bindingIndex = 0;

					while (bindingIndex < material.descriptorSetLayoutBindings.count()) {
						const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = material.descriptorSetLayoutBindings[bindingIndex];

						DEBUG_ASSERT(descriptorSetLayoutBinding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER);
						DEBUG_ASSERT(descriptorSetLayoutBinding.shaderStages == ShaderStage::FRAGMENT);

						materialDescriptorSetLayoutBindingImageDescriptor.layerCount = descriptorSetLayoutBinding.descriptorCount;

						#if defined(BUILD_MODE_DEBUG)
							name = Logger::format("Material [], binding []", materialIndex, bindingIndex);

							SET_DEBUG_NAME(materialDescriptorSetLayoutBindingImageDescriptor, name);
						#endif

						materialImages[materialIndex][bindingIndex] = createImage(materialDescriptorSetLayoutBindingImageDescriptor);
						materialImageAssets[materialIndex][bindingIndex] = sequence<ref<ImageAsset>>(materialReferences.count());

						materialReferenceIndex = 0;

						while (materialReferenceIndex < materialReferences.count()) {
							const MaterialReference& materialReference = materialReferences[materialReferenceIndex];
							const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(materialReference.descriptorImageAssetPaths[bindingIndex]);

							materialImageAssets[materialIndex][bindingIndex][materialReferenceIndex] = imageAsset;

							imageStagingBufferSize += imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4;

							imageCopyRegionCount += 1;
							materialReferenceIndex += 1;
						}

						bindingIndex += 1;
						materialBindingCount += 1;
					}

					materialIndex += 1;
				}

				materialDescriptorSets = getDevice()->allocateDescriptorSets(materialDescriptorSetLayouts);

				const ref<Buffer<uint8>> imageStagingBuffer = createStagingBuffer<uint8>(imageStagingBufferSize);

				uint8* imageMappedRange = imageStagingBuffer->map(0, imageStagingBuffer->getSize());
				uint64 imageMappedRangeOffset = 0;
				uint32 materialReferenceBindingImageIndex = 0;

				materialReferenceBindingImageViews = sequence<ref<ImageView>>(imageCopyRegionCount);

				sequence<BufferImageCopyRegion> imageCopyRegions(imageCopyRegionCount);
				sequence<DescriptorImage> descriptorImages(imageCopyRegionCount);
				sequence<WriteDescriptorSet> writeDescriptorSets(materialBindingCount);

				materialIndex = 0;
				materialBindingCount = 0;

				ImageViewDescriptor materialReferenceBindingImageViewDescriptor{};
				materialReferenceBindingImageViewDescriptor.mipLevelOffset = 0;
				materialReferenceBindingImageViewDescriptor.mipLevelCount = 1;
				materialReferenceBindingImageViewDescriptor.layerCount = 1;

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Write layered images");

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();

				while (materialIndex < materials.count()) {
					const Material& material = materials[materialIndex];
					const view<MaterialReference> materialReferences = scene->getMaterialReferences(materialIndex);

					bindingIndex = 0;

					while (bindingIndex < material.descriptorSetLayoutBindings.count()) {
						const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = material.descriptorSetLayoutBindings[bindingIndex];
						const ref<Image> image = materialImages[materialIndex][bindingIndex];
						const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materialReferences.count());

						sequence<BufferImageCopyRegion> imageCopyRegions(materialReferences.count());

						materialReferenceIndex = 0;

						while (materialReferenceIndex < materialReferences.count()) {
							const ref<ImageAsset> imageAsset = materialImageAssets[materialIndex][bindingIndex][materialReferenceIndex];

							DEBUG_ASSERT(imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM);

							sequence<uint8> data(imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4);

							convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &data[0], data.count());

							std::copy(data.begin(), data.end(), &imageMappedRange[imageMappedRangeOffset]);

							BufferImageCopyRegion& imageCopyRegion = imageCopyRegions[materialReferenceIndex];
							imageCopyRegion.bufferOffset = imageMappedRangeOffset;
							imageCopyRegion.imageOffsetX = 0;
							imageCopyRegion.imageOffsetY = 0;
							imageCopyRegion.imageOffsetZ = 0;
							imageCopyRegion.imageExtent = imageAsset->getExtent();
							imageCopyRegion.imageMipLevel = 0;
							imageCopyRegion.imageLayerOffset = materialReferenceIndex;
							imageCopyRegion.imageLayerCount = 1;

							materialReferenceBindingImageViewDescriptor.layerOffset = materialReferenceIndex;

							#if defined(BUILD_MODE_DEBUG)
								name = Logger::format("Material [], binding [], reference []", materialIndex, bindingIndex, materialReferenceIndex);

								SET_DEBUG_NAME(materialReferenceBindingImageViewDescriptor, name);
							#endif

							materialReferenceBindingImageViews[materialReferenceBindingImageIndex] = image->createView(materialReferenceBindingImageViewDescriptor);

							DescriptorImage& descriptorImage = descriptorImages[materialReferenceBindingImageIndex];
							descriptorImage.imageView = materialReferenceBindingImageViews[materialReferenceBindingImageIndex];
							descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
							descriptorImage.sampler = imageSampler;

							imageMappedRangeOffset += data.count();
							materialReferenceIndex += 1;
							materialReferenceBindingImageIndex += 1;
						}

						WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[bindingIndex];
						writeDescriptorSet.descriptorSet = materialDescriptorSets[materialIndex];
						writeDescriptorSet.bindingIndex = bindingIndex;
						writeDescriptorSet.descriptorImages = bindingDescriptorImages;

						commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
						commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);
						commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

						bindingIndex += 1;
						materialBindingCount += 1;
					}

					materialIndex += 1;
				}

				imageStagingBuffer->unmap();

				queue->submit({commandEncoder->end()});
				queue->waitIdle();

				getDevice()->updateDescriptorSets(writeDescriptorSets);
			}

			ref<Buffer<Camera>> createCameraBuffer() const {
				BufferDescriptor cameraBufferDescriptor{};
				cameraBufferDescriptor.size = sizeof(Camera);
				cameraBufferDescriptor.usage = BufferUsage::UNIFORM;
				cameraBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				cameraBufferDescriptor.persistent = true;

				SET_DEBUG_NAME(cameraBufferDescriptor, "Camera");

				return createBuffer<Camera>(cameraBufferDescriptor);
			}

			ref<Sampler> createImageSampler() const {
				SamplerDescriptor samplerDescriptor{};
				samplerDescriptor.magFilter = SamplerFilter::LINEAR;
				samplerDescriptor.minFilter = SamplerFilter::LINEAR;

				return createSampler(samplerDescriptor);
			}

			void updateDescriptorSets(uint64 vertexPositionComponentBufferSize, uint64 vertexPositionIndexBufferSize, uint64 vertexNormalComponentBufferSize, uint64 vertexNormalIndexBufferSize, uint64 vertexTextureCoordinateComponentBufferSize, uint64 vertexTextureCoordinateIndexBufferSize, uint64 vertexTangentComponentBufferSize) const {
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

				DescriptorBuffer vertexTangentComponentDescriptorBuffer{};
				vertexTangentComponentDescriptorBuffer.buffer = vertexBuffer;
				vertexTangentComponentDescriptorBuffer.offset = vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize + vertexTextureCoordinateIndexBufferSize;
				vertexTangentComponentDescriptorBuffer.size = vertexTangentComponentBufferSize;

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