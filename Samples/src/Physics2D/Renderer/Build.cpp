// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build(const PhysicsRendererBuildOptions2D& options) {
		sampleCount = options.sampleCount;

		build();

		const uint32 vertexCount = options.maxVertexCount + 4;
		const uint32 transformCount = options.maxTransformCount;
		const uint32 indexCount = vertexCount / 2 * 3;

		vertexBuffer = createVertexBuffer(vertexCount);
		transformBuffer = createTransformBuffer(transformCount);
		indexBuffer = createIndexBuffer(indexCount);
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

		const ref<DescriptorSetLayout> descriptorSetLayouts[] = {
			cameraViewDescriptorSetLayout,
			placeholderDescriptorSetLayout,
		};

		// TODO(base/ref): The ownership doesn't change if the sequence is constant (no operator= are called).
		sequence<ref<DescriptorSet>> descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);

		cameraViewDescriptorSet = descriptorSets[0];
		placeholderDescriptorSet = descriptorSets[1];

		createCameraViewPipelineLayout();
		createPlaceholderPipelineLayout();

		gridPipeline = createGridPipeline();
		circlePipeline = createCirclePipeline();
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
	}

	StaticMemoryDescriptor Physics2DRenderer::getStaticMemoryDescriptor() const {
		return {
			.buffers = view<ref<Buffer>>(&vertexBuffer, 4),
			.images = {},
		};
	}

	void Physics2DRenderer::postBuild() {
		transformBufferView = transformBuffer->createView<Transform2D>(0, transformBuffer->getSize());
		transformCount = 0;
		transformMappedRange = transformBufferView.map();

		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		cameraMappedRange = cameraView.map();
		cameraMappedRange->projectionAndZoomLevel.m[2].w = DEFAULT_ZOOM_LEVEL;

		updateCameraViewDescriptorSet();
		updatePlaceholderDescriptorSet();

		updateCameraProjection();
		updateViewport();
	}
}