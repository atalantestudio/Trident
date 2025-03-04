#include <imgui/imgui.h>

#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/2D/Renderer2D.hpp"

#include "EditorLayer.hpp"

namespace ProjectA {
	struct Position {
		vec3<float32> position;
	};

	struct Velocity {
		vec3<float32> velocity;
	};

	EditorLayer::EditorLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f) {}

	void EditorLayer::bind() {
		PROFILE_SCOPE(FUNC_SIG);

		checkerboardTexture = Texture2D::create("assets/textures/checkerboard.png");

		FramebufferDescriptor framebufferDescriptor;
		framebufferDescriptor.renderToSwapchain = false;
		framebufferDescriptor.sampleCount = 1;
		framebufferDescriptor.size = {1280.0f, 720.0f};

		framebuffer = Framebuffer::create(framebufferDescriptor);

		const Entity e1 = ecs.createEntity();
		const Entity e2 = ecs.createEntity();
		const Entity e3 = ecs.createEntity();
		ecs.defineComponent<Position>();
		ecs.defineComponent<Velocity>();
		Position pos;
		pos.position = {1.0f, 2.0f, 3.0f};
		ecs.setComponent<Position>(e1, pos);
		ecs.setComponent<Position>(e2, pos);
		ecs.setComponent<Position>(e3, pos);
	}

	void EditorLayer::unbind() {
		PROFILE_SCOPE(FUNC_SIG);

		const Entity e1 = 0;
		const Entity e2 = 1;
		const Entity e3 = 2;

		Position& pos1 = ecs.getComponent<Position>(e2);
		ComponentPool<Position>& pool = ecs.getComponentPool<Position>();

		for (EntityComponent<Position>& pos : pool.entityComponents) {
			pos.component.position *= 2.0f;

			ENGINE_DEBUG("Pos: []", pos.component.position);
		}
	}

	void EditorLayer::update(const float32 deltaTime) {
		PROFILE_SCOPE(FUNC_SIG);

		if (viewportWindowSize[0] > 0.0f && viewportWindowSize[1] > 0.0f && viewportWindowSize != framebuffer->descriptor.size) {
			framebuffer->resize(viewportWindowSize);
			cameraController.resize(viewportWindowSize);
		}

		if (viewportWindowFocused) {
			cameraController.update(deltaTime);
		}

		Renderer2D::resetStatistics();

		framebuffer->bind();

		//Renderer::getRendererApi()->setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		//Renderer::getRendererApi()->clear();

		Renderer2D::camera = &cameraController.getCamera();

		#if 1
			static float32 angle = 0.0f;

			angle += deltaTime;

			// Z axis is reversed (+ points towards the screen).
			// TODO: Inverse rotation direction?
			// Z rotation is clockwise against axis (0, 0, -1).
			// TODO: Fix black squares on shapes
			Renderer2D::beginScene();
			Renderer2D::drawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, lyah::radians(45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
			Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
			Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, color);
			Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {20.0f, 20.0f}, checkerboardTexture, 10.0f);
			Renderer2D::drawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, angle, checkerboardTexture, 20.0f);
			Renderer2D::endScene();

			Renderer2D::beginScene();

			for (float32 y = -5.0f; y < 5.0f; y += 0.5f) {
				for (float32 x = -5.0f; x < 5.0f; x += 0.5f) {
					vec2<float32> position = {x, y};
					vec2<float32> size = {0.45f, 0.45f};
					vec4<float32> color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};

					Renderer2D::drawQuad(position, size, color);
				}
			}

			Renderer2D::endScene();
		#endif

		framebuffer->unbind();
	}

	void EditorLayer::renderImGui() {
		PROFILE_SCOPE(FUNC_SIG);

		Renderer2D::Statistics statistics = Renderer2D::getStatistics();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, {0.0f, 0.0f}, dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					Application::get().close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		ImGui::Text("Per-frame statistics:");
		ImGui::Text("Draw call count: %d", statistics.drawCallCount);
		ImGui::Text("Quad count: %d", statistics.quadCount);
		ImGui::Text("Total vertex count: %d", statistics.getTotalVertexCount());
		ImGui::Text("Total index count: %d", statistics.getTotalIndexCount());
		ImGui::ColorEdit4("Color", color.m.m128_f32);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
		ImGui::Begin("Viewport");

		viewportWindowFocused = ImGui::IsWindowFocused();
		viewportWindowHovered = ImGui::IsWindowHovered();

		Application::get().imGuiLayer->blockEvents = !viewportWindowFocused || !viewportWindowHovered;

		const ImVec2 imGuiViewportWindowSize = ImGui::GetContentRegionAvail();

		viewportWindowSize = {imGuiViewportWindowSize.x, imGuiViewportWindowSize.y};

		const uint32 textureId = framebuffer->getColorAttachmentId();
		const vec2<float32> size = framebuffer->descriptor.size;

		ImGui::Image((void*) (std::uintptr_t) textureId, {size[0], size[1]}, {0.0f, 1.0f}, {1.0f, 0.0f});

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::onEvent(Event& event) {
		cameraController.onEvent(event);
	}
}