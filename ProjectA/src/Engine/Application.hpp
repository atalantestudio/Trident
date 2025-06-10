#pragma once

#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Timer/HighResolutionTimer.hpp"
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
			static std::string formatWindowName(std::string_view projectName, uint8 instanceCount);

		public:
			Application(const ApplicationManifest& applicationManifest);

			virtual ~Application();

			static const Window& getWindow();

			// Returns the current frame time, in seconds.
			static float64 getCurrentFrameTime();

			// Returns the average frames per second count.
			static float64 getFramesPerSecond();

			static uint32 getFrameIndex();

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
			static constexpr uint8 MAX_PARALLEL_INSTANCES = 5;
			static constexpr float64 FIXED_DELTA_TIME = 1.0 / 165.0;

		public:
			// Used by the default Renderer class to initialize in flight objects
			// (command encoders, fences, semaphores).
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			struct SharedMemoryData {
				uint8 instanceCount;
			};

		private:
			static Application* instance;

			static bool vSync;
			static bool looping;
			static bool windowResized;

			static float64 previousTime;
			static float64 frameTime;
			static float64 frameTimeAccumulator;

			static uint32 frameIndex;

			// In the range [0, MAX_FRAMES_IN_FLIGHT].
			static uint32 frameInFlightIndex;

		private:
			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			SharedMemoryData* sharedMemoryData = nullptr;
			HighResolutionTimer timer;
			Window* window = nullptr;
			PropertyFile* propertyFile = nullptr;
			LayerStack layerStack;
	};

	Application* createApplication();
}