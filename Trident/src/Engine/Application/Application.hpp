// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Process/Process.hpp"
#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Timer/HighResolutionTimer.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/LayerCompositor.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"

int main(int argc, char** argv);

namespace Trident {
	class Renderer;

	class Application {
		friend int ::main(int argc, char** argv);

		public:
			static float64 getFixedDeltaTime();

			static float64 getInverseFixedDeltaTime();

		public:
			explicit Application(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues);

			~Application();

			void initializePropertyFile();
			void initializeWindow(const ProjectDescriptor& projectDescriptor);
			void initializeGraphicsAdapter(const ProjectDescriptor& projectDescriptor);

			void allocateProcesses(uint64 count);

			const sequence<void*>& getConsoleArguments() const;

			void setConsoleArguments(uint32 argumentCount, char8** argumentValues);

			const PropertyFile* getPropertyFile() const;
			PropertyFile* getPropertyFile();

			bool hasLayers() const;

			// Returns the current frame time, in seconds.
			float64 getCurrentFrameTime() const;

			// Returns the average frames per second count.
			float64 getFramesPerSecond() const;

			uint32 getFrameIndex() const;

			const Window* getWindow() const;

			AssetLibrary* getAssetLibrary() const;

			inline const LayerCompositor* getLayerCompositor() const {
				return layerCompositor;
			}

			void postConstruct();

			sint32 runProcesses() const;

			virtual Renderer* getMainRenderer() const = 0;

		protected:
			template<typename LayerType, typename... Argument>
			void registerLayer(Argument&&... arguments) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				ref<LayerType> layer = allocateRef<LayerType>(std::forward<Argument>(arguments)...);
				layer->setApplication(this);

				layerCompositor->addLayer(std::move(layer));
			}

			template<typename LayerGroupType, typename... Argument>
			void registerLayerGroup(Argument&&... arguments) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				ref<LayerGroupType> layerGroup = allocateRef<LayerGroupType>(std::forward<Argument>(arguments)...);
				layerGroup->setApplication(this);

				layerCompositor->addLayerGroup(std::move(layerGroup));
			}

			template<typename ProcessType, typename... Argument>
			void registerProcess(Argument&&... arguments) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				ref<ProcessType> process = allocateRef<ProcessType>(std::forward<Argument>(arguments)...);
				process->setId(processCount);

				processes[processCount] = std::move(process);

				processCount += 1;
			}

		private:
			void loop();

			// Runs updates at a fixed time step, then renders a new frame.
			void processFrame();

			void update(float64 deltaTime, float64 inverseDeltaTime);

			void render(float64 remainingFrameTime);

			void resize();

			void close();

			void onWindowResize(WindowResizeEvent& event);

			void onWindowClose(WindowCloseEvent& event);

		private:
			static constexpr uint8 MAX_PARALLEL_INSTANCES = 5;
			static constexpr float64 FIXED_DELTA_TIME = 1.0 / 60.0;
			static constexpr float64 INVERSE_FIXED_DELTA_TIME = 1.0 / FIXED_DELTA_TIME;
			static constexpr uint32 MAX_UPDATES_PER_FRAME = 2;

		public:
			// Since Vulkan SDK 1.4.313, having less semaphores than swapchain images can cause the warning below to appear:
			// "VkSemaphore is being signaled by VkQueue, but it may still be in use by VkSwapchainKHR".
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

		private:
			struct SharedMemoryData {
				uint8 instanceCount;
			};

		protected:
			PropertyFile* propertyFile = nullptr;

		private:
			ApplicationProperties* properties = nullptr;
			ProjectType projectType;
			sequence<void*> consoleArguments;
			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			SharedMemoryData* sharedMemoryData = nullptr;
			HighResolutionTimer timer;
			AssetLibrary* assetLibrary = nullptr;
			Window* window = nullptr;
			LayerInput lastLayerInput;

		protected:
			ref<GraphicsAdapter> graphicsAdapter;

		private:
			ref<LayerCompositor> layerCompositor;
			sequence<ref<Process>> processes;
			uint32 processCount = 0;
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