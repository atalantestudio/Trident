#include "pch.hpp"

#include "RendererLayer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	RendererLayer::RendererLayer() {
		Geometry2DDescriptor polygonGeometryDescriptor;
		polygonGeometryDescriptor.vertices = {
			{-3.0f,   0.0f},
			{ 4.0f,   0.0f},
			{ 3.0f,  -1.0f},
			{-2.25f, -1.5f},
		};
		polygonGeometryDescriptor.indices = {
			0, 1, 2,
			0, 2, 3,
		};

		const Geometry2D polygonGeometry(polygonGeometryDescriptor);
		const uint32 polygonGeometryIndex = scene.registerGeometry(polygonGeometry);

		ColorMaterialDescriptor polygonMaterialDescriptor;
		polygonMaterialDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
		polygonMaterialDescriptor.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

		const ref<ColorMaterial> polygonMaterial = createRef<ColorMaterial>(polygonMaterialDescriptor);
		const uint32 polygonMaterialIndex = scene.registerMaterial(polygonMaterial);

		Object2DDescriptor polygonObjectDescriptor{};
		polygonObjectDescriptor.geometryIndex = polygonGeometryIndex;
		polygonObjectDescriptor.materialIndex = polygonMaterialIndex;
		polygonObjectDescriptor.debugName = "Polygon";

		Object2D& polygonObject = scene.registerObject(polygonObjectDescriptor);

		polygonObject.position = {0.45f, -0.625f};

		Geometry2DDescriptor triangleGeometryDescriptor;
		triangleGeometryDescriptor.vertices = {
			{0.0f, 0.0f},
			{0.0f, 1.0f},
			{6.0f, 0.0f},
		};
		triangleGeometryDescriptor.indices = {
			0, 1, 2,
		};

		const Geometry2D triangleGeometry(triangleGeometryDescriptor);
		const uint32 triangleGeometryIndex = scene.registerGeometry(triangleGeometry);

		ColorMaterialDescriptor triangleMaterialDescriptor;
		triangleMaterialDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
		triangleMaterialDescriptor.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

		const ref<ColorMaterial> triangleMaterial = createRef<ColorMaterial>(triangleMaterialDescriptor);
		const uint32 triangleMaterialIndex = scene.registerMaterial(triangleMaterial);

		Object2DDescriptor triangleObjectDescriptor{};
		triangleObjectDescriptor.geometryIndex = triangleGeometryIndex;
		triangleObjectDescriptor.materialIndex = triangleMaterialIndex;
		triangleObjectDescriptor.debugName = "Triangle";

		Object2D& triangleObject = scene.registerObject(triangleObjectDescriptor);

		triangleObject.position = {0.0f, 1.957f};
		triangleObject.rotation = lyah::radians(-30.0f);
		//triangleObject.angularVelocity = lyah::radians(0.4617f);
		triangleObject.angularVelocity = lyah::radians(32.26f);
		//triangleObject.angularVelocity = lyah::radians(70.0f);

		renderer.registerScene(scene);
	}

	void RendererLayer::update(float64 fixedDeltaTime) {
		const float32 t = static_cast<float32>(fixedDeltaTime);

		for (uint32 objectIndex = 0; objectIndex < scene.getObjectCount(); objectIndex += 1) {
			Object2D& A = scene[objectIndex];

			A.position += A.linearVelocity * t;
			A.rotation += A.angularVelocity * t;

			A.linearVelocity += A.linearAcceleration * t;
			A.angularVelocity += A.angularAcceleration * t;

			scene.updateTransform(A);

			renderer.writeInstanceTransform(objectIndex, A.transform);
		}
	}

	void RendererLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void RendererLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseWheelEvent>(BIND(onMouseWheel));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool RendererLayer::onMouseWheel(MouseWheelEvent& event) const {
		renderer.zoom(static_cast<float32>(event.getDelta()));

		return true;
	}

	bool RendererLayer::onWindowResize(WindowResizeEvent& event) const {
		const float32 width = static_cast<float32>(event.getWidth());
		const float32 height = static_cast<float32>(event.getHeight());

		renderer.resize(width, height);

		return true;
	}
}