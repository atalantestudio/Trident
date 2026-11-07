// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build(const PhysicsRendererBuildOptions2D& options) {
		sampleCount = options.sampleCount;
		maxCircleTransformCount = options.maxCircleTransformCount;
		circleTransformCount = 0;
		hulls = options.hulls;

		build();

		uint32 vertexCount = 4;
		uint32 transformCount = options.maxCircleTransformCount;
		uint32 triangleStripIndexCount = 6;
		uint32 maxLineStripIndexCount = 0;

		for (const PhysicsRendererBuildHull2D& hull : hulls) {
			vertexCount += hull.hullHeader->vertexCount;
			triangleStripIndexCount += hull.hullHeader->vertexCount;
			maxLineStripIndexCount = max(maxLineStripIndexCount, hull.hullHeader->vertexCount + 1);
			transformCount += hull.maxTransformCount;
		}

		lineStripIndexOffset = triangleStripIndexCount;

		vertexBuffer = createVertexBuffer(vertexCount);
		transformBuffer = createTransformBuffer(transformCount);
		indexBuffer = createIndexBuffer(triangleStripIndexCount + maxLineStripIndexCount);

		transformMappedRanges = sequence<Transform2D*>(hulls.count() + 1);
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
		transformBufferView = transformBuffer->createView<Transform2D>(0, transformBuffer->getSize());

		Transform2D* transformMappedRange = transformBufferView.map();

		transformMappedRanges = sequence<Transform2D*>(hulls.count() + 1);
		transformMappedRanges[0] = transformMappedRange;
		transformMappedRange += maxCircleTransformCount;

		uint32 hullIndex = 0;

		while (hullIndex < hulls.count()) {
			const PhysicsRendererBuildHull2D& hull = hulls[hullIndex];

			transformMappedRanges[hullIndex + 1] = transformMappedRange;
			transformMappedRange += hull.maxTransformCount;

			hullIndex += 1;
		}

		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		cameraMappedRange = cameraView.map();
		cameraMappedRange->projectionAndZoomLevel.m[2].w = DEFAULT_ZOOM_LEVEL;

		updateCameraViewDescriptorSet();
		// updatePlaceholderDescriptorSet();

		updateCameraProjection();
		updateViewport();
	}
}