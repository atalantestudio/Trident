#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/base.hpp"
#include "Engine/Filesystem/File/ConfigurationFile.hpp"
#include "Engine/Layer/LayerStack.hpp"
#include "Engine/Renderer/Swapchain.hpp"
#include "Engine/WindowOld.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		public:
			static Application& get();

		public:
			Application(const WindowDescriptor& windowDescriptor/*, const ConfigurationFile& configurationFile*/);

			virtual ~Application();

			const Window& getWindow() const;

			WindowOld& getWindowOld();

			const ref<RenderPass>& getRenderPass() const;

			const ref<Swapchain>& getSwapchain() const;

			void onEvent(Event& event);

			template<typename T, typename... U>
			void registerLayer(U&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				Layer* layer = new T(std::forward<U>(arguments)...);

				layerStack.pushLayer(layer);
			}

			template<typename T, typename... U>
			void registerOverlay(U&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				//Layer* overlay = new T(std::forward<U>(arguments)...);

				//layerStack.pushOverlay(overlay);
			}

			void close();

		private:
			void loop();

			void render();

			void resize();

			bool onWindowClose(WindowCloseEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		private:
			static Application* instance;

			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			Window window;
			scope<WindowOld> windowOld;

			LayerStack layerStack;

			// TODO: Use scope here?
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<CommandEncoder> commandEncoder;
			std::array<ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> availableImageSemaphores;
			std::array<ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> finishedRenderSemaphores;
			std::array<ref<Fence>, MAX_FRAMES_IN_FLIGHT> flightFences;

			bool looping = true;
			bool needsResize = false;
			bool minimized = false;
			float32 lastFrameTime = 0.0f;

			// Multiple of MAX_FRAMES_IN_FLIGHT.
			uint32 frameInFlightIndex = 0;
	};

	Application* createApplication();
}