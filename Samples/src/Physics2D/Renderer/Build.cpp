// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build(const PhysicsRendererBuildOptions2D& options) {
		sampleCount = options.sampleCount;
		cameraPosition = options.cameraPosition;
		zoomLevel = options.zoomLevel;
		maxCircleInstanceCount = options.maxCircleInstanceCount;
		circleInstanceCount = 0;
		hulls = options.hulls;
		contactCount = 0;
		maxContactCount = options.maxContactCount;

		build();

		sequence<UniqueVertexCount> uniqueVertexCountsDraft(hulls.count());
		uint32 vertexCount = 4;
		uint32 instanceCount = options.maxCircleInstanceCount;
		uint32 triangleStripIndexCount = 4;
		uint32 lineStripIndexCount = 0;
		uint32 uniqueVertexCountCount = 0;

		for (PhysicsRendererBuildHull2D& hull : hulls) {
			uint32 index = 0;

			while (index < uniqueVertexCountCount) {
				if (hull.hullHeader->vertexCount == uniqueVertexCountsDraft[index].vertexCount) {
					break;
				}

				index += 1;
			}

			UniqueVertexCount& uniqueVertexCount = uniqueVertexCountsDraft[index];

			if (index == uniqueVertexCountCount) {
				uniqueVertexCount.vertexCount = hull.hullHeader->vertexCount;
				uniqueVertexCount.triangleStripIndexOffset = triangleStripIndexCount;
				uniqueVertexCount.lineStripIndexOffset = lineStripIndexCount;

				triangleStripIndexCount += uniqueVertexCount.vertexCount;
				lineStripIndexCount += uniqueVertexCount.vertexCount + 1;

				hull.uniqueVertexCountIndex = index;

				uniqueVertexCountCount += 1;
			}

			vertexCount += hull.hullHeader->vertexCount;
			instanceCount += hull.maxInstanceCount;
		}

		uniqueVertexCounts = sequence<UniqueVertexCount>(uniqueVertexCountCount);

		std::copy_n(&uniqueVertexCountsDraft[0], uniqueVertexCountCount, &uniqueVertexCounts[0]);

		lineStripIndexOffset = triangleStripIndexCount * sizeof(uint16);

		vertexBuffer = createVertexBuffer(vertexCount);
		instanceBuffer = createInstanceBuffer(instanceCount);
		indexBuffer = createIndexBuffer(triangleStripIndexCount + lineStripIndexCount);
		contactBuffer = createContactBuffer(maxContactCount);
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
		contactPipeline = createContactPipeline();
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
			.buffers = view<ref<Buffer>>(&vertexBuffer, 5),
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
			const PhysicsRendererBuildHull2D hull = hulls[hullIndex];

			instanceMappedRanges[hullIndex + 1] = instanceMappedRange;
			instanceMappedRange += hull.maxInstanceCount;

			hullIndex += 1;
		}

		contactBufferView = contactBuffer->createView<vec2<float32>>(0, contactBuffer->getSize());
		contactMappedRange = contactBufferView.map();

		cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());
		cameraMappedRange = cameraView.map();
		cameraMappedRange->projection.m[2].w = zoomLevel;

		updateCameraViewDescriptorSet();
		// updatePlaceholderDescriptorSet();

		updateCameraProjection();
		updateViewport();
	}
}