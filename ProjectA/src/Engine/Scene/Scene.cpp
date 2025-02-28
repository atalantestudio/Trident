#include "pch.hpp"

#include "Engine/Scene/Scene.hpp"

namespace ProjectA {
	Scene::Scene() {
		struct Transform {
			mat4x4<float32> transform;

			Transform() = default;

			Transform(const mat4x4<float32>& transform) : transform(transform) {}

			Transform(const Transform& transform) = default;
		};

		const Entity entity = ecs.createEntity();

		ecs.allocateComponentPool<Transform>();
	}

	Scene::~Scene() {}
}