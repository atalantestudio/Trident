// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

#include "../../Physics2D/Renderer/CenterOfMassPipeline.hpp"
#include "../../Physics2D/Renderer/DistancePipeline.hpp"
#include "../../Physics2D/Renderer/EdgeFeaturePipeline.hpp"
#include "../../Physics2D/Renderer/GridPipeline.hpp"
#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"
#include "../../Physics2D/Renderer/RigidBodyFillPipeline.hpp"
#include "../../Physics2D/Renderer/RigidBodyPlaceholderCenterOfMassPipeline.hpp"
#include "../../Physics2D/Renderer/RigidBodyPlaceholderFillPipeline.hpp"
#include "../../Physics2D/Renderer/RigidBodyPlaceholderStrokePipeline.hpp"
#include "../../Physics2D/Renderer/RigidBodyStrokePipeline.hpp"
#include "../../Physics2D/Renderer/VertexFeaturePipeline.hpp"

namespace Trident {
	void Physics2DRenderer::build() {
		setupRenderPass();

		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			if (layerInput->swapchainImage == nullptr) {
				recreateMultisampleImage();
				recreateMultisampleImageView();
			}
		#endif

		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		createCameraBuffer();
		createViewBuffer();
		createPlaceholderBuffer();

		#if defined(SHOW_DISTANCE)
			createClosestPointBuffer();
		#endif

		#if defined(SHOW_FEATURES)
			createVertexFeatureBuffer();
			createEdgeFeatureBuffer();
		#endif

		createCameraViewDescriptorSetLayout();
		createPlaceholderDescriptorSetLayout();

		allocateDescriptorSets();

		updateCameraViewDescriptorSet();
		updatePlaceholderDescriptorSet();

		createCameraViewPipelineLayout();
		createPlaceholderPipelineLayout();

		gridPipeline = createGridPipeline();
		rigidBodyFillPipeline = createRigidBodyFillPipeline();
		rigidBodyStrokePipeline = createRigidBodyStrokePipeline();
		centerOfMassPipeline = createCenterOfMassPipeline();
		rigidBodyPlaceholderFillPipeline = createRigidBodyPlaceholderFillPipeline();
		rigidBodyPlaceholderStrokePipeline = createRigidBodyPlaceholderStrokePipeline();
		rigidBodyPlaceholderCenterOfMassPipeline = createRigidBodyPlaceholderCenterOfMassPipeline();

		#if defined(SHOW_DISTANCE)
			distancePipeline = createDistancePipeline();
		#endif

		#if defined(SHOW_FEATURES)
			vertexFeaturePipeline = createVertexFeaturePipeline();
			edgeFeaturePipeline = createEdgeFeaturePipeline();
		#endif

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::setupRenderPass() {
		renderPass = createRenderPass({
			.attachments = {
				// Color
				{
					.format = getSurface()->getFormat(),
					.sampleCount = PHYSICS_2D_RENDERER_SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::PRESENT_SRC,
				},

				// Depth stencil
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = PHYSICS_2D_RENDERER_SAMPLE_COUNT,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::UNDEFINED,
					.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
			},
			.subpasses = {
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
					.depthStencilAttachmentReference = {
						.index = 1,
						.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					},
				},
			},
		});
	}

	void Physics2DRenderer::recreateMultisampleImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		layerInput->swapchainImage = renderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = swapchainExtent.width,
					.height = swapchainExtent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Multisample attachment",
				#endif
			},
			0
		);
	}

	void Physics2DRenderer::recreateMultisampleImageView() {
		layerInput->swapchainImageView = layerInput->swapchainImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Multisample attachment",
			#endif
		});
	}

	void Physics2DRenderer::recreateDepthStencilImage() {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		depthStencilImage = renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = swapchainExtent.width,
					.height = swapchainExtent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Depth stencil attachment",
				#endif
			},
			0
		);
	}

	void Physics2DRenderer::recreateDepthStencilImageView() {
		depthStencilImageView = depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Depth stencil attachment",
			#endif
		});
	}

	void Physics2DRenderer::recreateFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.extent = swapchain->getExtent(),
		};

		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			framebufferDescriptor.imageViews = {layerInput->swapchainImageView, depthStencilImageView, nullptr};
		#else
			framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
		#endif

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
				framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);
			#else
				framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);
			#endif

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void Physics2DRenderer::createCameraBuffer() {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		cameraBuffer = createDynamicBuffer({
			.size = ceil(sizeof(Camera), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});

		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

		cameraMappedRange = cameraView.map();
		cameraMappedRange->zoomLevel = DEFAULT_ZOOM_LEVEL;
	}

	void Physics2DRenderer::createViewBuffer() {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		viewBuffer = createDynamicBuffer({
			.size = ceil(sizeof(View), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "View",
			#endif
		});

		const BufferView<View> viewView = viewBuffer->createView<View>(0, viewBuffer->getSize());

		viewMappedRange = viewView.map();
	}

	void Physics2DRenderer::createPlaceholderBuffer() {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		placeholderBuffer = createDynamicBuffer({
			.size = ceil(sizeof(Placeholder), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Placeholder",
			#endif
		});

		const BufferView<Placeholder> placeholderView = placeholderBuffer->createView<Placeholder>(0, placeholderBuffer->getSize());

		placeholderMappedRange = placeholderView.map();
		placeholderMappedRange->offset = {0.0f, 0.0f};
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::createClosestPointBuffer() {
			closestPointBuffer = createDynamicBuffer({
				.size = 2 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Closest point",
				#endif
			});

			const BufferView<Vertex> closestPointView = closestPointBuffer->createView<Vertex>(0, closestPointBuffer->getSize());

			closestPointMappedRange = closestPointView.map();
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::createVertexFeatureBuffer() {
			vertexFeatureBuffer = createDynamicBuffer({
				.size = 2 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Vertex feature",
				#endif
			});

			const BufferView<Vertex> vertexFeatureView = vertexFeatureBuffer->createView<Vertex>(0, vertexFeatureBuffer->getSize());

			vertexFeatureMappedRange = vertexFeatureView.map();
		}

		void Physics2DRenderer::createEdgeFeatureBuffer() {
			edgeFeatureBuffer = createDynamicBuffer({
				.size = 4 * sizeof(Vertex),
				.usage = BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
				.persistent = true,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Edge feature",
				#endif
			});

			const BufferView<Vertex> edgeFeatureView = edgeFeatureBuffer->createView<Vertex>(0, edgeFeatureBuffer->getSize());

			edgeFeatureMappedRange = edgeFeatureView.map();
		}
	#endif

	void Physics2DRenderer::createCameraViewDescriptorSetLayout() {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},

			// View
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		cameraViewDescriptorSetLayout = createDescriptorSetLayout(bindings);
	}

	void Physics2DRenderer::createPlaceholderDescriptorSetLayout() {
		const DescriptorSetLayoutBinding bindings[] = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},
		};

		placeholderDescriptorSetLayout = createDescriptorSetLayout(bindings);
	}

	void Physics2DRenderer::createCameraViewPipelineLayout() {
		cameraViewPipelineLayout = createPipelineLayout({
			.descriptorSetLayouts = {cameraViewDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera view",
			#endif
		});
	}

	void Physics2DRenderer::createPlaceholderPipelineLayout() {
		placeholderPipelineLayout = createPipelineLayout({
			.descriptorSetLayouts = {cameraViewDescriptorSetLayout, placeholderDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Placeholder",
			#endif
		});
	}

	void Physics2DRenderer::allocateDescriptorSets() {
		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			cameraViewDescriptorSetLayout,
			placeholderDescriptorSetLayout,
		};

		// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
		sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

		cameraViewDescriptorSet = descriptorSets[0];
		placeholderDescriptorSet = descriptorSets[1];
	}

	void Physics2DRenderer::setScene(const Scene2D* scene) {
		this->scene = scene;

		const view<uint8> dataBuffer = scene->getDataBuffer();
		const view<Geometry2D> geometries = scene->getGeometries();
		const view<Hull2D> hulls = scene->getHulls();
		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();
		const uint64 rigidBodyBufferSize = rigidBodyComponentPool.componentCount * sizeof(RigidBody);
		const ref<Buffer> stagingBuffer = createDynamicBuffer({
			.size = dataBuffer.count() + rigidBodyBufferSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const BufferView<uint8> dataStagingView = stagingBuffer->createView<uint8>(0, dataBuffer.count());
		const BufferView<RigidBody> rigidBodyStagingView = stagingBuffer->createView<RigidBody>(dataBuffer.count(), rigidBodyBufferSize);

		std::copy(dataBuffer.begin(), dataBuffer.end(), dataStagingView.map());

		RigidBody* stagingRigidBodyMappedRange = rigidBodyStagingView.map();
		uint32 rigidBodyIndex = 0;

		while (rigidBodyIndex < rigidBodyComponentPool.componentCount) {
			const RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[rigidBodyIndex];
			const GeometryComponent geometryComponent = geometryComponentPool.components[rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]];
			const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
			const Hull2D& hull = hulls[geometry.hullIndex];

			RigidBody& rigidBody = stagingRigidBodyMappedRange[rigidBodyIndex];
			rigidBody.world = rigidBodyComponent.world;

			if (hull.type != Hull2DType::POLYGON) {
				// Make hulls that don't support wireframe visualization more visible.
				rigidBody.fillColor = Color(0x8b9476ff).value;
			}
			else {
				rigidBody.fillColor = Color(0xf4fedbff).value;
			}

			rigidBody.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
			rigidBody.depth = static_cast<float32>(rigidBodyIndex) / rigidBodyComponentPool.componentCount;
			rigidBody.selected = 0;

			rigidBodyIndex += 1;
		}

		stagingBuffer->unmap();

		vertexBuffer = createDynamicBuffer({
			.size = scene->getDynamicVertexCount() * sizeof(Vertex),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
		indexBuffer = createDynamicBuffer({
			.size = scene->getDynamicIndexCount() * sizeof(uint32),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
		rigidBodyBuffer = createDynamicBuffer({
			.size = rigidBodyBufferSize,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rigid body",
			#endif
		});
		centerOfMassBuffer = createDynamicBuffer({
			.size = rigidBodyComponentPool.componentCount * sizeof(CenterOfMass),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Center of mass",
			#endif
		});

		const BufferView<RigidBody> rigidBodyView = rigidBodyBuffer->createView<RigidBody>(0, rigidBodyBuffer->getSize());
		const BufferView<CenterOfMass> centerOfMassView = centerOfMassBuffer->createView<CenterOfMass>(0, centerOfMassBuffer->getSize());

		rigidBodyMappedRange = rigidBodyView.map();
		centerOfMassMappedRange = centerOfMassView.map();

		const ref<Queue> transferQueue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = transferQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy buffers",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, {
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = vertexBuffer->getSize(),
			},
		});
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, {
			{
				.sourceOffset = vertexBuffer->getSize(),
				.destinationOffset = 0,
				.size = indexBuffer->getSize(),
			},
		});
		commandEncoder->copyBufferToBuffer(stagingBuffer, rigidBodyBuffer, {
			{
				.sourceOffset = vertexBuffer->getSize() + indexBuffer->getSize(),
				.destinationOffset = 0,
				.size = rigidBodyBufferSize,
			},
		});

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		transferQueue->submit(commandBuffer);
		transferQueue->waitIdle();
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const {
			closestPointMappedRange[0].position = {closestPointA[0], closestPointA[1]};
			closestPointMappedRange[1].position = {closestPointB[0], closestPointB[1]};
		}
	#endif

	void Physics2DRenderer::setWorld(Entity entity, const mat3x3<float32>& world) const {
		rigidBodyMappedRange[entity].world = world;
	}

	void Physics2DRenderer::setPlaceholderOffset(vec2<float32> offset) const {
		placeholderMappedRange->offset = {offset[0], offset[1]};
	}

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::registerFeature(const Feature2D& feature) {
			switch (feature.vertexCount) {
				case 1: {
					vertexFeatureMappedRange[vertexFeatureCount].position = {feature.vertex[0], feature.vertex[1]};
					vertexFeatureCount += 1;

					break;
				}
				case 2: {
					edgeFeatureMappedRange[edgeFeatureCount * 2 + 0].position = {feature.edge[0][0], feature.edge[0][1]};
					edgeFeatureMappedRange[edgeFeatureCount * 2 + 1].position = {feature.edge[1][0], feature.edge[1][1]};
					edgeFeatureCount += 1;

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}
		}
	#endif

	void Physics2DRenderer::selectEntity(Entity entity) {
		if (selectedEntity != max<Entity>()) {
			rigidBodyMappedRange[selectedEntity].selected = 0;
		}

		selectedEntity = entity;

		if (selectedEntity != max<Entity>()) {
			rigidBodyMappedRange[selectedEntity].selected = 1;
		}
	}

	void Physics2DRenderer::updateCameraViewDescriptorSet() const {
		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		const BufferView<View> viewView = viewBuffer->createView<View>(0, viewBuffer->getSize());

		const WriteDescriptorSet writeDescriptorSets[] = {
			{
				.descriptorSet = cameraViewDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = cameraView,
			},
			{
				.descriptorSet = cameraViewDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = viewView,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void Physics2DRenderer::updatePlaceholderDescriptorSet() const {
		const BufferView<Placeholder> placeholderView = placeholderBuffer->createView<Placeholder>(0, placeholderBuffer->getSize());

		const WriteDescriptorSet writeDescriptorSets[] = {
			{
				.descriptorSet = placeholderDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = placeholderView,
			},
		};

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void Physics2DRenderer::updateCentersOfMass() const {
		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		const uint32 componentCount = rigidBodyComponentPool.componentCount;
		const float32 inverseComponentCount = 1.0f / componentCount;

		uint32 componentIndex = 0;

		while (componentIndex < componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

			centerOfMassMappedRange[entity].position = {rigidBodyComponent.position[0], rigidBodyComponent.position[1], componentIndex * inverseComponentCount};
			componentIndex += 1;
		}
	}

	void Physics2DRenderer::updateCameraProjection() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 w = static_cast<float32>(swapchainExtent.width);
		const float32 h = static_cast<float32>(swapchainExtent.height);
		const vec2<float32> O = -cameraPosition / (vec2<float32>(w, h) * 0.5f);

		// TODO: lyah::mat<3, 3, float32>::orthographic
		cameraMappedRange->projection = {
			2.0f / w,  0.0f,      0.0f,
			0.0f,      2.0f / h,  0.0f,
			O[0],      O[1],      1.0f,
		};
	}

	void Physics2DRenderer::updateViewport() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		viewMappedRange->viewport.x = static_cast<float32>(swapchainExtent.width);
		viewMappedRange->viewport.y = static_cast<float32>(swapchainExtent.height);
	}

	void Physics2DRenderer::zoom(float32 delta) {
		if (delta != 0.0f) {
			delta /= abs(delta);
		}

		cameraMappedRange->zoomLevel = clamp(cameraMappedRange->zoomLevel + delta, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

		updateCameraProjection();
	}

	void Physics2DRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.width);
		const float32 height = static_cast<float32>(swapchainExtent.height);

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffers[getSwapchainImageIndex()],
			.clearValues = {
				{1.0f, 1.0f, 1.0f, 1.0f},
				{
					.depth = 0.0f,
					.stencil = 0,
				},
				#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
					{0.0f, 0.0f, 0.0f, 1.0f},
				#endif
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Grid
		{
			commandEncoder->bindGraphicsPipeline(gridPipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
			commandEncoder->draw(3, 1, 0, 0);
		}

		const view<Geometry2D> geometries = scene->getGeometries();
		const view<Hull2D> hulls = scene->getHulls();
		const ComponentPool<GeometryComponent>& geometryComponentPool = scene->ecs.getComponentPool<GeometryComponent>();

		uint32 componentIndex = 0;

		// Rigid body fill
		{
			commandEncoder->bindGraphicsPipeline(rigidBodyFillPipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
			commandEncoder->bindIndexBuffer<uint32>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const GeometryComponent geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];

				// TODO: Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
				commandEncoder->drawIndexed(geometry.getIndexRange().count, 1, geometry.getIndexRange().offset, geometry.getVertexPositionComponentRange().offset / 2, entity);

				componentIndex += 1;
			}
		}

		componentIndex = 0;

		// Rigid body stroke
		{
			commandEncoder->bindGraphicsPipeline(rigidBodyStrokePipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
			commandEncoder->bindIndexBuffer<uint32>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const GeometryComponent geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
				const Hull2D& hull = hulls[geometry.hullIndex];

				// TODO: Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
				commandEncoder->drawIndexed(hull.lineIndexRange.count, 1, hull.lineIndexRange.offset, geometry.getVertexPositionComponentRange().offset / 2, entity);

				componentIndex += 1;
			}
		}

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		// Center of mass
		{
			updateCentersOfMass();

			commandEncoder->bindGraphicsPipeline(centerOfMassPipeline);
			commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
			commandEncoder->draw(rigidBodyComponentPool.componentCount, 1, 0, 0);
		}

		/* #if defined(SHOW_DISTANCE)
			renderDistance(commandEncoder);
		#endif

		#if defined(SHOW_FEATURES)
			renderFeatures(commandEncoder);
		#endif */

		if (selectedEntity != max<Entity>()) {
			const ref<DescriptorSet> descriptorSets[2] = {cameraViewDescriptorSet, placeholderDescriptorSet};
			const GeometryComponent geometryComponent = scene->ecs.getComponent<GeometryComponent>(selectedEntity);
			const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
			const Hull2D& hull = hulls[geometry.hullIndex];

			// Rigid body placeholder center of mass
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderCenterOfMassPipeline);
				commandEncoder->bindVertexBuffer(0, centerOfMassBuffer, 0);
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, descriptorSets);
				commandEncoder->draw(1, 1, selectedEntity, 0);
			}

			// Rigid body placeholder fill
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderFillPipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
				commandEncoder->bindIndexBuffer<uint32>(indexBuffer, 0);
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, descriptorSets);
				commandEncoder->drawIndexed(geometry.getIndexRange().count, 1, geometry.getIndexRange().offset, geometry.getVertexPositionComponentRange().offset / 2, selectedEntity);
			}

			// Rigid body placeholder stroke
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderStrokePipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
				commandEncoder->bindIndexBuffer<uint32>(indexBuffer, 0);
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, descriptorSets);
				commandEncoder->drawIndexed(hull.lineIndexRange.count, 1, hull.lineIndexRange.offset, geometry.getVertexPositionComponentRange().offset / 2, selectedEntity);
			}
		}

		commandEncoder->endRenderPass();
	}

	#if defined(SHOW_DISTANCE)
		void Physics2DRenderer::renderDistance(ref<CommandEncoder> commandEncoder) const {
			commandEncoder->bindGraphicsPipeline(distancePipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
			commandEncoder->bindVertexBuffer(0, closestPointBuffer, 0);
			commandEncoder->draw(2, 1, 0, 0);
		}
	#endif

	#if defined(SHOW_FEATURES)
		void Physics2DRenderer::renderFeatures(ref<CommandEncoder> commandEncoder) const {
			if (vertexFeatureCount > 0) {
				commandEncoder->bindGraphicsPipeline(vertexFeaturePipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
				commandEncoder->bindVertexBuffer(0, vertexFeatureBuffer, 0);
				commandEncoder->draw(vertexFeatureCount, 1, 0, 0);
			}

			if (edgeFeatureCount > 0) {
				commandEncoder->bindGraphicsPipeline(edgeFeaturePipeline);
				commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
				commandEncoder->bindVertexBuffer(0, edgeFeatureBuffer, 0);
				commandEncoder->draw(edgeFeatureCount * 2, 1, 0, 0);
			}
		}
	#endif

	void Physics2DRenderer::resize(WindowResizeEvent& event) {
		#if PHYSICS_2D_RENDERER_SAMPLE_COUNT > 1
			if (layerInput->swapchainImage == nullptr) {
				recreateMultisampleImage();
				recreateMultisampleImageView();
			}
		#endif

		recreateDepthStencilImage();
		recreateDepthStencilImageView();
		recreateFramebuffers();

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::clearRegisteredData() {
		#if defined(SHOW_DISTANCE)
			closestPointMappedRange[0].position = {0.0f, 0.0f};
			closestPointMappedRange[1].position = {0.0f, 0.0f};
		#endif

		#if defined(SHOW_FEATURES)
			vertexFeatureCount = 0;
			edgeFeatureCount = 0;
		#endif
	}
}