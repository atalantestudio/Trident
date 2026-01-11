#pragma once

#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Timer/HighResolutionTimer.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Layer/LayerStack.hpp"
#include "Engine/Process/Process.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		public:
			static float64 getFixedDeltaTime();

			static float64 getInverseFixedDeltaTime();

			static sequence<char8> formatWindowName(view<char8> projectName, uint8 instanceCount);

		public:
			explicit Application(const ProjectDescriptor& projectDescriptor);

			virtual ~Application();

			const sequence<void*>& getConsoleArguments() const;

			void setConsoleArguments(uint32 argumentCount, char8** argumentValues);

			bool hasLayers() const;

			// Returns the current frame time, in seconds.
			float64 getCurrentFrameTime() const;

			// Returns the average frames per second count.
			float64 getFramesPerSecond() const;

			uint32 getFrameIndex() const;

			const Window* getWindow() const;

			const AssetLibrary* getAssetLibrary() const;

			void postConstruct();

			sint32 runProcesses() const;

		protected:
			template<typename LayerType, typename... Argument>
			void registerLayer(Argument&&... arguments) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const ref<Layer> layer = createRef<LayerType>(std::forward<Argument>(arguments)...);

				layer->setApplication(this);
				layer->build();

				layerStack.pushLayer(layer);
			}

			template<typename ProcessType>
			void registerProcess() {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const uint32 processId = static_cast<uint32>(processes.size());

				ref<Process> process = createRef<ProcessType>();
				process->setId(processId);

				processes.emplace_back(process);
			}

		private:
			void loop();

			// Runs updates at a fixed time step, then renders a new frame.
			void processFrame();

			void update(float64 deltaTime, float64 inverseDeltaTime);

			void render(float64 remainingFrameTime);

			void resize();

			void close();

			void propagateEventFrontToBack(Event& event);
			void propagateEventBackToFront(Event& event);

			void onWindowResize(WindowResizeEvent& event);

			void onWindowClose(WindowCloseEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		private:
			static constexpr uint8 MAX_PARALLEL_INSTANCES = 5;

			// TODO: 165 FPS causes the dominos in the domino physics sample to fall on the wrong side.
			static constexpr float64 FIXED_DELTA_TIME = 1.0 / 60.0;

			static constexpr float64 INVERSE_FIXED_DELTA_TIME = 1.0 / FIXED_DELTA_TIME;

		public:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			struct SharedMemoryData {
				uint8 instanceCount;
			};

		private:
			ApplicationProperties* properties = nullptr;
			ProjectType projectType;

			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			SharedMemoryData* sharedMemoryData = nullptr;
			HighResolutionTimer timer;
			Window* window = nullptr;
			LayerStack layerStack;
			LayerInput lastLayerInput;
			std::vector<ref<Process>> processes;

			sequence<void*> consoleArguments;

			AssetLibrary* assetLibrary = nullptr;

			uint32 frameIndex = 0;

			// In the range [0, MAX_FRAMES_IN_FLIGHT].
			uint32 frameInFlightIndex = 0;

			float64 previousTime = 0.0;
			float64 frameTime = 0.0;
			float64 frameTimeAccumulator = 0.0;

			bool looping = true;
			bool windowResized = false;
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues);
}