#include "pch.hpp"

#include "RendererLayer.hpp"

#include "Engine/API/Geometry/PolytopeGeometry2D.hpp"
#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Physics/Integrator.hpp"
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
		objectADescriptor.geometryIndex = scene.registerGeometry<PolytopeGeometry2D>(polygonGeometryDescriptor);
		objectADescriptor.materialIndex = scene.registerMaterial<ColorMaterial>(material1Descriptor);

		Object2D& objectA = scene.registerObject(objectADescriptor);

		objectA.position = {0.45f, -0.625f};

		objectA.updateTransform();

		Object2DDescriptor objectBDescriptor{};
		objectBDescriptor.geometryIndex = scene.registerGeometry<PolytopeGeometry2D>(triangleGeometryDescriptor);
		objectBDescriptor.materialIndex = scene.registerMaterial<ColorMaterial>(material2Descriptor);

		Object2D& objectB = scene.registerObject(objectBDescriptor);

		objectB.position = {0.0f, 1.957f};
		objectB.rotation = lyah::radians(-30.0f);
		objectB.angularVelocity = lyah::radians(0.4617f);

		objectB.updateTransform();

		renderer.registerScene(scene);
	}

	void RendererLayer::update(float64 fixedDeltaTime) {
		renderer.clearClosestFeatures();

		Integrator::integrate(scene, static_cast<float32>(fixedDeltaTime), &gjkResult);

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

	bool RendererLayer::onMouseDown(MouseDownEvent& event) {
		if (event.getButton() == MouseButton::LEFT) {
			selectedObjectIndex = hoveredObjectIndex;
		}

		return true;
	}

	bool RendererLayer::onMouseMove(MouseMoveEvent& event) {
		const float32 x = static_cast<float32>(event.getX());
		const float32 y = static_cast<float32>(event.getY());

		const vec2<float32> previousPosition = renderer.getNdcMousePosition();

		renderer.setMousePosition(x, y);

		if (selectedObjectIndex != std::numeric_limits<uint32>::max()) {
			const vec2<float32>& position = renderer.getNdcMousePosition();
			const vec2<float32> positionDelta = position - previousPosition;

			scene[selectedObjectIndex].position += positionDelta;
		}

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

	bool RendererLayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return true;
	}
}