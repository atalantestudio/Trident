#include "pch.hpp"

#include "RendererLayer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	RendererLayer::RendererLayer() {
		/// Geometries ///

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

		Geometry2DDescriptor triangleGeometryDescriptor;
		triangleGeometryDescriptor.vertices = {
			{0.0f, 0.0f},
			{0.0f, 1.0f},
			{6.0f, 0.0f},
		};
		triangleGeometryDescriptor.indices = {
			0, 1, 2,
		};

		Geometry2DDescriptor squareGeometryDescriptor;
		squareGeometryDescriptor.vertices = {
			{-1.0f, -1.0f},
			{-1.0f,  1.0f},
			{ 1.0f,  1.0f},
			{ 1.0f, -1.0f},
		};
		squareGeometryDescriptor.indices = {
			0, 1, 2,
			0, 2, 3,
		};

		/// Materials ///

		ColorMaterialDescriptor material1Descriptor;
		material1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
		material1Descriptor.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

		ColorMaterialDescriptor material2Descriptor;
		material2Descriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
		material2Descriptor.strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

		/// Objects ///

		Object2DDescriptor objectADescriptor{};
		objectADescriptor.geometryIndex = scene.registerGeometry(polygonGeometryDescriptor);
		objectADescriptor.materialIndex = scene.registerMaterial<ColorMaterial>(material1Descriptor);

		Object2D& objectA = scene.registerObject(objectADescriptor);

		//objectA.position = {0.45f, -0.625f};
		objectA.position = {-4.45f, -3.625f};

		scene.updateTransform(objectA);

		Object2DDescriptor objectBDescriptor{};
		objectBDescriptor.geometryIndex = scene.registerGeometry(triangleGeometryDescriptor);
		objectBDescriptor.materialIndex = scene.registerMaterial<ColorMaterial>(material2Descriptor);

		Object2D& objectB = scene.registerObject(objectBDescriptor);

		objectB.position = {0.0f, 1.957f};
		objectB.rotation = lyah::radians(-30.0f);
		objectB.angularVelocity = lyah::radians(32.26f);

		scene.updateTransform(objectB);

		renderer.registerScene(scene);
	}

	void RendererLayer::update(float64 fixedDeltaTime) {
		renderer.clearClosestFeatures();

		const float32 t = static_cast<float32>(fixedDeltaTime);

		for (uint32 i = 0; i < scene.getObjectCount() - 1; i += 1) {
			for (uint32 j = i + 1; j < scene.getObjectCount(); j += 1) {
				Object2D& A = scene[i];
				Object2D& B = scene[j];

				gjkResult = GJK2D(A, B);

				advanceSimulation(A, t);
				advanceSimulation(B, t);
			}
		}

		checkHover();
	}

	void RendererLayer::preRender() {
		for (uint32 objectIndex = 0; objectIndex < scene.getObjectCount(); objectIndex += 1) {
			const Object2D& object = scene[objectIndex];

			renderer.writeInstanceTransform(objectIndex, object.transform);
		}

		renderer.setSelectedObjectIndex(selectedObjectIndex);

		renderer.setClosestPoints(gjkResult.closestA, gjkResult.closestB);
		renderer.registerClosestFeature(gjkResult.closestFeatureA);
		renderer.registerClosestFeature(gjkResult.closestFeatureB);
	}

	void RendererLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void RendererLayer::checkHover() {
		hoveredObjectIndex = std::numeric_limits<uint32>::max();

		const vec2<float32>& p = renderer.getNdcMousePosition();

		// Check if the mouse hovers over an object.
		for (sint32 objectIndex = static_cast<sint32>(scene.getObjectCount() - 1); objectIndex >= 0; objectIndex -= 1) {
			const Object2D& A = scene[objectIndex];

			if (GJK2DPoint(A, p).distance <= 0.0f) {
				hoveredObjectIndex = objectIndex;

				break;
			}
		}
	}

	void RendererLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
		eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onMouseUp));
		eventDispatcher.dispatch<MouseWheelEvent>(BIND(onMouseWheel));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	void RendererLayer::advanceSimulation(Object2D& A, float32 t) const {
		A.position += A.linearVelocity * t;
		A.rotation += A.angularVelocity * t;

		A.linearVelocity += A.linearAcceleration * t;
		A.angularVelocity += A.angularAcceleration * t;

		scene.updateTransform(A);
	}

	bool RendererLayer::onMouseDown(MouseDownEvent& event) {
		if (event.getButton() == MouseButton::LEFT) {
			selectedObjectIndex = hoveredObjectIndex;
		}

		return true;
	}

	bool RendererLayer::onMouseMove(MouseMoveEvent& event) {
		const float32 x = static_cast<float32>(event.getX());
		const float32 y = static_cast<float32>(event.getY());

		renderer.setMousePosition(x, y);

		return true;
	}

	bool RendererLayer::onMouseUp(MouseUpEvent& event) {
		if (event.getButton() == MouseButton::LEFT) {
			selectedObjectIndex = std::numeric_limits<uint32>::max();
		}

		return true;
	}

	bool RendererLayer::onMouseWheel(MouseWheelEvent& event) {
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