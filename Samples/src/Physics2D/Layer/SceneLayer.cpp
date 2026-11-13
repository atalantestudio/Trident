// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "SampleApplication.hpp"
#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::initialize() {
		rendererOptions.sampleCount = ImageSampleCount::x1;
		rendererOptions.cameraPosition = {0.0f, 8.0f};
		rendererOptions.zoomLevel = 36.0f;
		rendererOptions.maxContactCount = 2048;

		PhysicsSimulation2DDescriptor simulationDescriptor;
		simulationDescriptor.deltaTime = static_cast<float32>(getApplication()->getFixedDeltaTime());
		simulationDescriptor.gravity = {0.0f, -9.80665f};
		simulationDescriptor.impulseIterationCount = 10;

		const view<char8> SampleName = getApplication()->getPropertyFile()->getProperty<view<char8>>("SampleName");

		if (SampleName == "box") {
			LoadBoxSample(simulationDescriptor);
		}
		else if (SampleName == "boxpyramid") {
			LoadBoxPyramidSample(simulationDescriptor);
		}
		else if (SampleName == "boxstacking") {
			LoadBoxStackingSample(simulationDescriptor);
		}
		else if (SampleName == "circle") {
			// TODO(physics/2d): Fix circle demo.
			LoadCircleSample(simulationDescriptor);
		}
		// TODO(physics/2d): Add circlesliding demo.
		else if (SampleName == "domino") {
			LoadDominoSample(simulationDescriptor);
		}
		else if (SampleName == "friction") {
			LoadFrictionSample(simulationDescriptor);
		}
		// TODO(physics/2d): Add polygon demo.
		else if (SampleName == "swing") {
			LoadSwingSample(simulationDescriptor);
		}
		else {
			ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleName);
		}

		renderer.setLayer(this);
		renderer.build(rendererOptions);
	}

	StaticMemoryDescriptor Physics2DSceneLayer::getStaticMemoryDescriptor() const {
		return renderer.getStaticMemoryDescriptor();
	}

	void Physics2DSceneLayer::postConstruct() {
		renderer.postBuild();
		renderer.encodeOneTimeCommands();
	}

	void Physics2DSceneLayer::preRender() {
		tick(simulation);

		// checkHover();

		renderer.resetCounters();

		// TODO(physics/2d/test)
		// constexpr vec3<float32> color = {0.956863, 0.996078, 0.858824};
		constexpr vec3<float32> color = {0.0f, 0.2078f, 0.2471f};

		uint32 shapeIndex = 0;

		while (shapeIndex < simulation.usedShapeCount) {
			const Shape2D& shape = simulation.shapes[shapeIndex];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			switch (shape.type) {
				case ShapeType2D::CIRCLE: {
					renderer.setInstance(0, rigidBody, color, shapeIndex);

					break;
				}
				case ShapeType2D::CAPSULE: {
					ENGINE_ERROR_UNIMPLEMENTED();

					break;
				}
				case ShapeType2D::CONVEX_HULL: {
					renderer.setInstance(shape.hull->userIndex, rigidBody, color, shapeIndex);

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			shapeIndex += 1;
		}

		uint32 contactCount = 0;

		for (const std::pair<const CollisionPair2D, ContactPair2D>& pair : simulation.contactPairs) {
			const ContactManifold2D& contactManifold = pair.second.manifold;

			if (contactCount + contactManifold.contactCount >= rendererOptions.maxContactCount) {
				break;
			}

			uint32 contactIndex = 0;

			while (contactIndex < contactManifold.contactCount) {
				renderer.setContact(contactManifold.contacts[contactIndex].position);

				contactIndex += 1;
			}
		}

		/* renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<Entity>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		} */

		if (draggingCamera) {
			renderer.updateCameraProjection();
		}
	}

	void Physics2DSceneLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void Physics2DSceneLayer::checkHover() {
		/* hoveredEntity = max<Entity>();

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		const Hull2D mouseHull = Hull2D::point();

		RigidBody2DComponent mouseRigidBodyComponent{};
		mouseRigidBodyComponent.position = renderer.cameraPosition + mousePosition;
		mouseRigidBodyComponent.updateWorld(mouseHull.centroid);

		sint32 componentIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount) - 1;

		while (componentIndex >= 0) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(entity);
			const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];
			const Hull2D& hull = scene->getHulls()[geometry.hullIndex];
			const RigidBody2DComponent& rigidBodyComponent = scene->ecs.getComponent<RigidBody2DComponent>(entity);

			if (GJK2DConditional(rigidBodyComponent, mouseRigidBodyComponent, hull, mouseHull)) {
				hoveredEntity = entity;

				break;
			}

			componentIndex -= 1;
		} */
	}

	void Physics2DSceneLayer::registerHullForRender(const HullHeader2D* hullHeader) {
		PhysicsRendererBuildHull2D& hull = rendererOptions.hulls[hullHeader->userIndex - 1];
		hull.hullHeader = hullHeader;
		hull.maxInstanceCount = 128;
	}

	void Physics2DSceneLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);
		eventDispatcher.dispatch<KeyDownEvent>(BIND(onEvent));
		eventDispatcher.dispatch<KeyUpEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseDownEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseMoveEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseWheelEvent>(BIND(onEvent));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onEvent));
	}

	bool Physics2DSceneLayer::onEvent(KeyDownEvent& event) {
		keyMap.update(event);

		return true;
	}

	bool Physics2DSceneLayer::onEvent(KeyUpEvent& event) {
		keyMap.update(event);

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseDownEvent& event) {
		const Window* window = getApplication()->getWindow();
		const Extent2D extent = window->getExtent();

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(extent) * 0.5f / renderer.getZoomLevel());
		mouseDelta = {0.0f, 0.0f};

		if (event.getButton() == MouseButton::RIGHT && selectedEntity == max<Entity>()) {
			draggingCamera = true;
		}

		/* if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<Entity>() && !draggingCamera) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		} */

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseMoveEvent& event) {
		const Window* window = getApplication()->getWindow();
		const Extent2D extent = window->getExtent();
		const vec2<float32> previousMousePosition = mousePosition;

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(extent) * 0.5f / renderer.getZoomLevel());
		mouseDelta = mousePosition - previousMousePosition;

		if (draggingCamera) {
			renderer.cameraPosition -= mouseDelta;
		}

		/* if (selectedEntity != max<Entity>()) {
			placeholderOffset += mouseDelta;
		} */

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseUpEvent& event) {
		if (event.getButton() == MouseButton::RIGHT && draggingCamera) {
			draggingCamera = false;
		}

		/* if (event.getButton() == MouseButton::LEFT && selectedEntity != max<Entity>()) {
			const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(selectedEntity);
			const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];
			const Hull2D& hull = scene->getHulls()[geometry.hullIndex];

			RigidBody2DComponent& rigidBody = scene->ecs.getComponent<RigidBody2DComponent>(selectedEntity);
			rigidBody.position += placeholderOffset;
			rigidBody.updateWorld(hull.centroid);

			selectedEntity = max<Entity>();
		} */

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseWheelEvent& event) {
		renderer.zoom(static_cast<float32>(event.getDelta()));

		return true;
	}

	bool Physics2DSceneLayer::onEvent(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}
}