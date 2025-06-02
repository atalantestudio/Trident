#pragma once

#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"
#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/API/Timer/HighResolutionTimer.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/base.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Layer/LayerStack.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	// TODO: Rename to ProjectManifest?
	struct ApplicationManifest {
		std::string_view projectName;

		// TODO: Handle custom window icon.
	};

	class Application {
		public:
			static Application& get();

			static std::string getWindowName(std::string_view projectName, uint8 instanceCount);

		public:
			Application(const ApplicationManifest& applicationManifest);

			virtual ~Application();

			const Window& getWindow() const;

			// Returns the current frame time, in seconds.
			float64 getCurrentFrameTime() const;

			// Returns the average frames per second count.
			float64 getFramesPerSecond() const;

			uint32 getFrameInFlightIndex() const;

			template<typename T, typename... U>
			void registerLayer(U&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				const ref<Layer> layer = createRef<T>(std::forward<U>(arguments)...);

				layerStack.pushLayer(layer);
			}

			template<typename T, typename... U>
			void registerOverlay(U&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				const ref<Layer> overlay = createRef<T>(std::forward<U>(arguments)...);

				layerStack.pushOverlay(overlay);
			}

			void close();

		private:
			void loop();

			// Runs updates at a fixed time step, then renders a new frame.
			void processFrame();

			void update(float64 deltaTime);

			void preRender();

			void render(float64 remainingFrameTime);

			void postRender();

			void resize();

			void onEvent(Event& event);

			void onWindowResize(WindowResizeEvent& event);

			void onWindowClose(WindowCloseEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
			static constexpr float64 RENDERS_PER_SECOND = 165.0;

			static constexpr float64 fixedDeltaTime = lyah::rcp(165.0);

		private:
			struct SharedMemoryData {
				uint8 instanceCount;
			};

		private:
			static Application* instance;

		private:
			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			SharedMemoryData* sharedMemoryData = nullptr;
			HighResolutionTimer timer;
			Window* window = nullptr;

			PropertyFile* propertyFile = nullptr;

			LayerStack layerStack;

			bool looping = true;
			bool windowResized = false;

			float64 previousTime = 0.0;
			float64 frameTime = 0.0;
			float64 frameTimeAccumulator = 0.0;

			// Multiple of MAX_FRAMES_IN_FLIGHT.
			uint32 frameInFlightIndex = 0;
	};

	Application* createApplication();
}