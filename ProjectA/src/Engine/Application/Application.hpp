#pragma once

#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Timer/HighResolutionTimer.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationManifest.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Layer/LayerStack.hpp"
#include "Engine/Process/Process.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		public:
			static std::string formatWindowName(std::string_view projectName, uint8 instanceCount);

		public:
			Application(const ApplicationManifest& applicationManifest);

			virtual ~Application();

			static const Window& getWindow();

			static float64 getFixedDeltaTime();

			// Returns the current frame time, in seconds.
			static float64 getCurrentFrameTime();

			// Returns the average frames per second count.
			static float64 getFramesPerSecond();

			static uint32 getFrameIndex();

			void postConstruct();

			bool hasGraphicalProcesses() const;

			const sequence<void*>& getConsoleArguments() const;

			void setConsoleArguments(uint32 argumentCount, char** argumentValues);

		protected:
			template<typename ProcessType, typename... Argument>
			void registerProcess(Argument&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				const ref<Process> process = createRef<ProcessType>(std::forward<Argument>(arguments)...);

				processes.emplace_back(process);
			}

			template<typename LayerType, typename... Argument>
			void registerLayer(Argument&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				const ref<Layer> layer = createRef<LayerType>(std::forward<Argument>(arguments)...);

				layer->build();

				layerStack.pushLayer(layer);
			}

			// TODO: Remove support for overlays.
			template<typename OverlayType, typename... Argument>
			void registerOverlay(Argument&&... arguments) {
				PROFILE_SCOPE(FUNC_SIG);

				const ref<Layer> overlay = createRef<OverlayType>(std::forward<Argument>(arguments)...);

				layerStack.pushOverlay(overlay);
			}

		private:
			void loop();

			// Runs updates at a fixed time step, then renders a new frame.
			void processFrame();

			void update(float64 deltaTime, float64 inverseDeltaTime);

			void render(float64 remainingFrameTime);

			void resize();

			void close();

			void onEvent(Event& event);

			void onWindowResize(WindowResizeEvent& event);

			void onWindowClose(WindowCloseEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		private:
			static constexpr uint8 MAX_PARALLEL_INSTANCES = 5;
			static constexpr float64 FIXED_DELTA_TIME = 1.0 / 60.0;
			static constexpr float64 INVERSE_FIXED_DELTA_TIME = 1.0 / FIXED_DELTA_TIME;

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

			static ApplicationProperties* properties;

		private:
			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			SharedMemoryData* sharedMemoryData = nullptr;
			HighResolutionTimer timer;
			Window* window = nullptr;
			LayerStack layerStack;
			std::vector<ref<Process>> processes;

			sequence<void*> consoleArguments;
	};

	Application* createApplication(uint32 argumentCount, char** argumentValues);
}