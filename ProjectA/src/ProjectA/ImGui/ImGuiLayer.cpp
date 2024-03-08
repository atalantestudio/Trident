#include "pch.hpp"
#include "ImGuiLayer.hpp"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "ProjectA/Application.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace ProjectA {
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}

	void ImGuiLayer::attach() {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::detach() {
		 ImGui::DestroyContext();
	}

	void ImGuiLayer::update() {
		ImGuiIO& io = ImGui::GetIO();
		Application& application = Application::get();
		WindowInterface& window = application.getWindow();

		io.DisplaySize = ImVec2(window.getWidth(), window.getHeight());

		float currentDeltaTime = (float)glfwGetTime();

		// Check that deltaTime > 0 in case the time is not set yet (the first frame)
		io.DeltaTime = deltaTime > 0.0f ? (currentDeltaTime - deltaTime) : (1.0f / 60.0f);

		deltaTime = currentDeltaTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;

		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::listen(Event& event) {
		EventDispatcher dispatcher(event);

		dispatcher.dispatch<KeyPressEvent>(BIND_FUNCTION(ImGuiLayer::onKeyPress));
		dispatcher.dispatch<KeyReleaseEvent>(BIND_FUNCTION(ImGuiLayer::onKeyRelease));
		dispatcher.dispatch<KeyTypeEvent>(BIND_FUNCTION(ImGuiLayer::onKeyType));
		dispatcher.dispatch<MouseButtonPressEvent>(BIND_FUNCTION(ImGuiLayer::onMouseButtonPress));
		dispatcher.dispatch<MouseButtonReleaseEvent>(BIND_FUNCTION(ImGuiLayer::onMouseButtonRelease));
		dispatcher.dispatch<MouseMoveEvent>(BIND_FUNCTION(ImGuiLayer::onMouseMove));
		dispatcher.dispatch<MouseScrollEvent>(BIND_FUNCTION(ImGuiLayer::onMouseScroll));
		dispatcher.dispatch<WindowResizeEvent>(BIND_FUNCTION(ImGuiLayer::onWindowResize));
	}

	bool ImGuiLayer::onKeyPress(KeyPressEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddKeyEvent(ImGuiKey(event.getKeyCode()), true);
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}

	bool ImGuiLayer::onKeyRelease(KeyReleaseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddKeyEvent(ImGuiKey(event.getKeyCode()), false);

		return false;
	}

	bool ImGuiLayer::onKeyType(KeyTypeEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		int keyCode = event.getKeyCode();

		if (keyCode > 0 && keyCode < 0x10000) {
			io.AddInputCharacter((unsigned short) keyCode);
		}

		return false;
	}

	bool ImGuiLayer::onMouseButtonPress(MouseButtonPressEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddMouseButtonEvent(event.getButton(), true);

		return false;
	}

	bool ImGuiLayer::onMouseButtonRelease(MouseButtonReleaseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddMouseButtonEvent(event.getButton(), false);

		return false;
	}

	bool ImGuiLayer::onMouseMove(MouseMoveEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddMousePosEvent(event.getX(), event.getY());

		return false;
	}

	bool ImGuiLayer::onMouseScroll(MouseScrollEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddMouseWheelEvent(event.getXOffset(), event.getYOffset());

		return false;
	}

	bool ImGuiLayer::onWindowResize(WindowResizeEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(event.getWidth(), event.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		glViewport(0, 0, event.getWidth(), event.getHeight());

		return false;
	}
}