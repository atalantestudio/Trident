#include <imgui/imgui.h>

#include "ProjectA/Renderer/2D/Renderer2D.hpp"

#include "SandboxLayer.hpp"

static const char* map =
	"WWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWDDDDDDWWWWWWWWWWW"
	"WWWWWDDDDDDDDDDDWWWWWWWW"
	"WWWWDDDDDDDDDDDDDDDWWWWW"
	"WWWDDDDDDDDDDDDDDDDDDWWW"
	"WWDDDDWWWDDDDDDDDDDDDWWW"
	"WDDDDDWWWDDDDDDDDDDDDDWW"
	"WWDDDDDDDDDDDDDDDDDDDWWW"
	"WWWWDDDDDDDDDDDDDDDDWWWW"
	"WWWWWDDDDDDDDDDDDDDWWWWW"
	"WWWWWWDDDDDDDDDDDWWWWWWW"
	"WWWWWWWDDDDDDDDDWWWWWWWW"
	"WWWWWWWWWWDDDDWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWW";
static const ProjectA::vec2<ProjectA::float32> mapSize = {24.0f, static_cast<ProjectA::float32>(std::strlen(map)) / 24.0f};

SandboxLayer::SandboxLayer() : Layer("Sandbox"), cameraController(1280.0f / 720.0f), particleSystem(100000) {}

void SandboxLayer::bind() {
	PROFILE_SCOPE(__FUNCSIG__);

	checkerboardTexture = ProjectA::Texture2D::create("assets/textures/checkerboard.png");
	rpgSpriteSheetTexture = ProjectA::Texture2D::create("assets/textures/RPGpack_sheet_2X.png");
	nullSprite = ProjectA::Subtexture::createSprite(rpgSpriteSheetTexture, {0.0f, 0.0f}, {128.0f, 128.0f});

	// Dirt
	sprites['D'] = ProjectA::Subtexture::createSprite(rpgSpriteSheetTexture, {6.0f, 11.0f}, {128.0f, 128.0f});

	// Water
	sprites['W'] = ProjectA::Subtexture::createSprite(rpgSpriteSheetTexture, {11.0f, 11.0f}, {128.0f, 128.0f});

	particleProperties.startColor = lyah::vec4<ProjectA::float32>(254.0f, 212.0f, 123.0f, 255.0f) / 255.0f;
	particleProperties.endColor = lyah::vec4<ProjectA::float32>(254.0f, 109.0f, 41.0f, 255.0f) / 255.0f;
	particleProperties.startSize = 0.5f;
	particleProperties.endSize = 0.0f;
	particleProperties.sizeBias = 0.3f;
	particleProperties.lifetime = 5.0f;
	particleProperties.position = {0.0f, 0.0f};
	particleProperties.linearVelocity = {0.0f, 0.0f};
	particleProperties.linearVelocityBias = {3.0f, 1.0f};
}

void SandboxLayer::unbind() {
	PROFILE_SCOPE(__FUNCSIG__);
}

void SandboxLayer::update(ProjectA::float32 deltaTime) {
	PROFILE_SCOPE(__FUNCSIG__);

	cameraController.update(deltaTime);

	ProjectA::Renderer2D::resetStatistics();

	{
		PROFILE_SCOPE("Pre-draw");

		ProjectA::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		ProjectA::RenderCommand::clear();
	}

	#if 0
		static ProjectA::float32 angle = 0.0f;

		angle += deltaTime;

		// Z axis is reversed (+ points towards the screen).
		// TODO: Inverse rotation direction?
		// Z rotation is clockwise against axis (0, 0, -1).
		// TODO: Fix black squares on shapes
		ProjectA::Renderer2D::beginScene(cameraController.getCamera());
		ProjectA::Renderer2D::drawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, lyah::radians(45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		ProjectA::Renderer2D::drawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		ProjectA::Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {20.0f, 20.0f}, checkerboardTexture, 10.0f);
		ProjectA::Renderer2D::drawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, angle, checkerboardTexture, 20.0f);
		ProjectA::Renderer2D::endScene();

		ProjectA::Renderer2D::beginScene(cameraController.getCamera());

		for (ProjectA::float32 y = -5.0f; y < 5.0f; y += 0.5f) {
			for (ProjectA::float32 x = -5.0f; x < 5.0f; x += 0.5f) {
				lyah::vec2<ProjectA::float32> position = {x, y};
				lyah::vec2<ProjectA::float32> size = {0.45f, 0.45f};
				lyah::vec4<ProjectA::float32> color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};

				ProjectA::Renderer2D::drawQuad(position, size, color);
			}
		}

		ProjectA::Renderer2D::endScene();
	#endif

	cameraController.setZoomLevel(12.0f);

	ProjectA::Renderer2D::beginScene(cameraController.getCamera());

	for (ProjectA::uint32 y = 0; y < static_cast<ProjectA::uint32>(mapSize[1]); y++) {
		for (ProjectA::uint32 x = 0; x < static_cast<ProjectA::uint32>(mapSize[0]); x++) {
			const char spriteId = map[y * static_cast<ProjectA::uint32>(mapSize[0]) + x];
			ProjectA::ref<ProjectA::Subtexture> sprite;
			auto found = sprites.find(spriteId);

			if (found != sprites.end()) {
				sprite = found->second;
			}
			else {
				sprite = nullSprite;
			}

			ProjectA::vec2<ProjectA::float32> position = {static_cast<ProjectA::float32>(x), static_cast<ProjectA::float32>(y)};
			position -= mapSize / 2.0f;

			ProjectA::Renderer2D::drawQuad({position[0], position[1], 0.0f}, {1.0f, 1.0f}, sprite);
		}
	}

	ProjectA::Renderer2D::endScene();

	/* if (ProjectA::Input::isMouseButtonPressed(ProjectA::MouseButton::BUTTON_LEFT)) {
		const lyah::vec2<ProjectA::float32> mousePosition = ProjectA::Input::getMousePosition();
		const ProjectA::float32 width = static_cast<ProjectA::float32>(ProjectA::Application::get().getWindow().getWidth());
		const ProjectA::float32 height = static_cast<ProjectA::float32>(ProjectA::Application::get().getWindow().getHeight());

		const lyah::vec4<ProjectA::float32>& bounds = cameraController.getBounds();
		const lyah::vec3<ProjectA::float32>& position = cameraController.getPosition();

		const ProjectA::float32 boundsWidth = bounds[1] - bounds[0];
		const ProjectA::float32 boundsHeight = bounds[3] - bounds[2];

		const ProjectA::float32 x = (mousePosition[0] / width) * boundsWidth - boundsWidth * 0.5f;
		const ProjectA::float32 y = -(mousePosition[1] / height) * boundsHeight + boundsHeight * 0.5f;

		particleProperties.position = {position[0] + x, position[1] + y};

		for (ProjectA::uint32 i = 0; i < 50; i++) {
			particleSystem.emitParticle(particleProperties);
		}
	}

	particleSystem.update(deltaTime);
	particleSystem.render(cameraController.getCamera()); */
}

void SandboxLayer::renderImGui() {
	PROFILE_SCOPE(__FUNCSIG__);

	ProjectA::Renderer2D::Statistics statistics = ProjectA::Renderer2D::getStatistics();

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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit")) {
				ProjectA::Application::get().close();
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

		const ProjectA::uint32 textureId = checkerboardTexture->getId();

		ImGui::Image((void*) textureId, {256.0f, 256.0f});
	ImGui::End();

	ImGui::End();
}

void SandboxLayer::onEvent(ProjectA::Event& event) {
	cameraController.onEvent(event);
}