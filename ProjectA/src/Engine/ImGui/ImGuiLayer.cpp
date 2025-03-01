#include "pch.hpp"

#include <glfw/glfw3.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_vulkan.h>

#include "Engine/Application.hpp"
#include "Engine/ImGui/ImGuiLayer.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	void ImGuiLayer::bind() {
		PROFILE_SCOPE(FUNC_SIG);

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		if (Renderer::graphicsApi != GraphicsAPI::OPENGL) {
			ENGINE_WARN("Only OpenGL ImGui layers are supported.");

			return;
		}

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::unbind() {
		PROFILE_SCOPE(FUNC_SIG);

		if (Renderer::graphicsApi != GraphicsAPI::OPENGL) {
			return;
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
	}

	void ImGuiLayer::begin() {
		PROFILE_SCOPE(FUNC_SIG);

		if (Renderer::graphicsApi != GraphicsAPI::OPENGL) {
			return;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
	}

	void ImGuiLayer::end() {
		PROFILE_SCOPE(FUNC_SIG);

		if (Renderer::graphicsApi != GraphicsAPI::OPENGL) {
			return;
		}

		ImGuiIO& io = ImGui::GetIO();
		const Window& window = Application::get().getWindow();

		io.DisplaySize = ImVec2(static_cast<float32>(window.getWidth()), static_cast<float32>(window.getHeight()));

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* currentContext = glfwGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(currentContext);
		}
	}

	void ImGuiLayer::onEvent(Event& event) {
		if (Renderer::graphicsApi != GraphicsAPI::OPENGL) {
			return;
		}

		if (!blockEvents) {
			return;
		}

		const ImGuiIO& io = ImGui::GetIO();

		event.handled |= event.hasCategory(EventCategory::KEYBOARD) & io.WantCaptureKeyboard;
		event.handled |= event.hasCategory(EventCategory::MOUSE) & io.WantCaptureMouse;
	}
}