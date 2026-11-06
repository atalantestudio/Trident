// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::createViewBuffer() {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		viewBuffer = createDynamicBuffer({
			.size = ceilPowerOf2(sizeof(View), alignment),
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
			.size = ceilPowerOf2(sizeof(Placeholder), alignment),
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

		cameraViewDescriptorSetLayout = createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}

	void Physics2DRenderer::createPlaceholderDescriptorSetLayout() {
		const DescriptorSetLayoutBinding bindings[] = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},
		};

		placeholderDescriptorSetLayout = createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Placeholder",
			#endif
		});
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

	void Physics2DRenderer::setScene(const Scene2D* scene) {
		this->scene = scene;

		const view<uint8> dataBuffer = scene->getDataBuffer();
		const view<Geometry2D> geometries = scene->getGeometries();
		const view<Hull2D> hulls = scene->getHulls();
		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();
		const ComponentPool<Geometry2DComponent>& geometryComponentPool = scene->ecs.getComponentPool<Geometry2DComponent>();
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
			const Geometry2DComponent& geometryComponent = geometryComponentPool.components[rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]];
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
		void Physics2DRenderer::setClosestPoints(vec2<float32> closestPointA, vec2<float32> closestPointB) const {
			closestPointMappedRange[0].position = closestPointA;
			closestPointMappedRange[1].position = closestPointB;
		}
	#endif

	void Physics2DRenderer::setWorld(Entity entity, const mat3x3<float32>& world) const {
		rigidBodyMappedRange[entity].world = world;
	}

	void Physics2DRenderer::setPlaceholderOffset(vec2<float32> offset) const {
		placeholderMappedRange->offset = offset;
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
		const vec2<float32> viewport = vec2<float32>(getSwapchain()->getExtent());
		const vec2<float32> origin = -cameraPosition / (viewport * 0.5f);
		const mat3x3<float32> P = orthographic(origin, viewport);
		const float32 zoomLevel = getZoomLevel();

		cameraMappedRange->projectionAndZoomLevel = {{
			{P[0].x, P[0].y, P[0].z, 1.0f},
			{P[1].x, P[1].y, P[1].z, 1.0f},
			{P[2].x, P[2].y, P[2].z, zoomLevel},
		}};
	}

	void Physics2DRenderer::updateViewport() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		viewMappedRange->viewport.x = static_cast<float32>(swapchainExtent.x);
		viewMappedRange->viewport.y = static_cast<float32>(swapchainExtent.y);
	}

	void Physics2DRenderer::zoom(float32 delta) {
		if (delta != 0.0f) {
			delta /= abs(delta);
		}

		cameraMappedRange->projectionAndZoomLevel.m[2].w = clamp(getZoomLevel() + delta, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

		updateCameraProjection();
	}

	void Physics2DRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(swapchainExtent.x);
		const float32 height = static_cast<float32>(swapchainExtent.y);

		// TODO(physics/2d/renderer): Store the clear values inside the renderer.
		sequence<ClearValue> clearValues(2 + (sampleCount != ImageSampleCount::x1));
		clearValues[0] = {1.0f, 1.0f, 1.0f, 1.0f};
		clearValues[1] = {
			.depth = 0.0f,
			.stencil = 0,
		};

		if (sampleCount != ImageSampleCount::x1) {
			clearValues[2] = {0.0f, 0.0f, 0.0f, 1.0f};
		}

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffers[getSwapchainImageIndex()],
			.clearValues = clearValues,
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, swapchainExtent);

		// Grid
		{
			commandEncoder->bindGraphicsPipeline(gridPipeline);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
			commandEncoder->draw(3, 1, 0, 0);
		}

		// Circle
		{
			commandEncoder->bindGraphicsPipeline(circlePipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindVertexBuffer(1, transformBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R16_UINT>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);
			commandEncoder->drawIndexed(6, transformCount, 0, 0, 0);
		}

		commandEncoder->endRenderPass();

		// TODO(wip)
		return;

		const view<Geometry2D> geometries = scene->getGeometries();
		const view<Hull2D> hulls = scene->getHulls();
		const ComponentPool<Geometry2DComponent>& geometryComponentPool = scene->ecs.getComponentPool<Geometry2DComponent>();

		uint32 componentIndex = 0;

		// Rigid body fill
		{
			commandEncoder->bindGraphicsPipeline(rigidBodyFillPipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const Geometry2DComponent& geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];

				// TODO(physics/2d/materials): Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
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
			commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(cameraViewPipelineLayout, 0, cameraViewDescriptorSet);

			while (componentIndex < geometryComponentPool.componentCount) {
				const Entity entity = geometryComponentPool.componentIndexToEntity[componentIndex];
				const Geometry2DComponent& geometryComponent = geometryComponentPool.components[componentIndex];
				const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
				const Hull2D& hull = hulls[geometry.hullIndex];

				// TODO(physics/2d/materials): Restore materials (test colors were 0xf4fedbff and 0xe6ddffff).
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
			const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(selectedEntity);
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
				commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
				commandEncoder->bindDescriptorSets(placeholderPipelineLayout, 0, descriptorSets);
				commandEncoder->drawIndexed(geometry.getIndexRange().count, 1, geometry.getIndexRange().offset, geometry.getVertexPositionComponentRange().offset / 2, selectedEntity);
			}

			// Rigid body placeholder stroke
			{
				commandEncoder->bindGraphicsPipeline(rigidBodyPlaceholderStrokePipeline);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, rigidBodyBuffer, 0);
				commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
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
		if (sampleCount != ImageSampleCount::x1) {
			colorImage = createColorImage();
			colorImageView = createColorImageView();
		}

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();

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