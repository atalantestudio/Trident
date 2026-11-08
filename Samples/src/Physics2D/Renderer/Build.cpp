// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build(const PhysicsRendererBuildOptions2D& options) {
		sampleCount = options.sampleCount;
		maxCircleInstanceCount = options.maxCircleInstanceCount;
		circleInstanceCount = 0;
		hulls = options.hulls;

		build();

		uint32 vertexCount = 4;
		uint32 instanceCount = options.maxCircleInstanceCount;
		uint32 triangleStripIndexCount = 6;
		uint32 lineStripIndexCount = 0;

		for (const PhysicsRendererBuildHull2D& hull : hulls) {
			vertexCount += hull.hullHeader->vertexCount;
			triangleStripIndexCount += hull.hullHeader->vertexCount;
			lineStripIndexCount = max(lineStripIndexCount, hull.hullHeader->vertexCount + 1);
			instanceCount += hull.maxInstanceCount;
		}

		lineStripIndexOffset = triangleStripIndexCount * sizeof(uint16);

		vertexBuffer = createVertexBuffer(vertexCount);
		instanceBuffer = createInstanceBuffer(instanceCount);
		indexBuffer = createIndexBuffer(triangleStripIndexCount + lineStripIndexCount);
	}

	void Physics2DRenderer::build() {
		renderPass = createRenderPass();

		if (sampleCount != ImageSampleCount::x1) {
			colorImage = createColorImage();
			colorImageView = createColorImageView();
		}

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();

		cameraBuffer = createCameraBuffer();

		createViewBuffer();
		// createPlaceholderBuffer();

		#if defined(SHOW_DISTANCE)
			// createClosestPointBuffer();
		#endif

		#if defined(SHOW_FEATURES)
			// createVertexFeatureBuffer();
			// createEdgeFeatureBuffer();
		#endif

		createCameraViewDescriptorSetLayout();
		// createPlaceholderDescriptorSetLayout();

		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			cameraViewDescriptorSetLayout,
			// placeholderDescriptorSetLayout,
		};

		// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
		sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

		cameraViewDescriptorSet = descriptorSets[0];
		// placeholderDescriptorSet = descriptorSets[1];

		createCameraViewPipelineLayout();
		// createPlaceholderPipelineLayout();

		gridPipeline = createGridPipeline();
		circlePipeline = createCirclePipeline();
		shapeFillPipeline = createShapeFillPipeline();
		shapeStrokePipeline = createShapeStrokePipeline();
		// centerOfMassPipeline = createCenterOfMassPipeline();
		// rigidBodyPlaceholderFillPipeline = createRigidBodyPlaceholderFillPipeline();
		// rigidBodyPlaceholderStrokePipeline = createRigidBodyPlaceholderStrokePipeline();
		// rigidBodyPlaceholderCenterOfMassPipeline = createRigidBodyPlaceholderCenterOfMassPipeline();

		#if defined(SHOW_DISTANCE)
			// distancePipeline = createDistancePipeline();
		#endif

		#if defined(SHOW_FEATURES)
			// vertexFeaturePipeline = createVertexFeaturePipeline();
			// edgeFeaturePipeline = createEdgeFeaturePipeline();
		#endif
	}

	StaticMemoryDescriptor Physics2DRenderer::getStaticMemoryDescriptor() const {
		return {
			.buffers = view<ref<Buffer>>(&vertexBuffer, 4),
			.images = {},
		};
	}

	void Physics2DRenderer::postBuild() {
		instanceBufferView = instanceBuffer->createView<PhysicsRendererInstance2D>(0, instanceBuffer->getSize());

		PhysicsRendererInstance2D* instanceMappedRange = instanceBufferView.map();

		instanceMappedRanges = sequence<PhysicsRendererInstance2D*>(hulls.count() + 1);
		instanceMappedRanges[0] = instanceMappedRange;
		instanceMappedRange += maxCircleInstanceCount;

		uint32 hullIndex = 0;

		while (hullIndex < hulls.count()) {
			const PhysicsRendererBuildHull2D& hull = hulls[hullIndex];

			instanceMappedRanges[hullIndex + 1] = instanceMappedRange;
			instanceMappedRange += hull.maxInstanceCount;

			hullIndex += 1;
		}

		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		cameraMappedRange = cameraView.map();
		cameraMappedRange->projection.m[2].w = DEFAULT_ZOOM_LEVEL;

		updateCameraViewDescriptorSet();
		// updatePlaceholderDescriptorSet();

		updateCameraProjection();
		updateViewport();
	}
}