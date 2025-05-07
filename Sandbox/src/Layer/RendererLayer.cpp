#include "pch.hpp"

#include "RendererLayer.hpp"

#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	RendererLayer::RendererLayer() {
		Scene2D scene;

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

		Object2D polygonObject(polygonObjectDescriptor);
		const uint32 polygonObjectIndex = scene.registerObject(polygonObject);

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

		Object2D triangleObject(triangleObjectDescriptor);
		const uint32 triangleObjectIndex = scene.registerObject(triangleObject);

		triangleObject.position = {0.45f, -0.625f};

		renderer.registerScene(scene);
	}

	vec4<float32> RendererLayer::getClearColor() const {
		return {1.0f, 1.0f, 1.0f, 1.0f};
	}

	void RendererLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}
}