#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/API/SharedMemory/SharedMemory.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Mutex.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/base.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Layer/LayerStack.hpp"
#include "Engine/Renderer/Swapchain.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	struct ApplicationDescriptor {
		std::string projectName;
	};

	class Application {
		public:
			static Application& get();

			static PropertyFile loadPropertyFile();

			static std::string getWindowName(const std::string& projectName, uint8 instanceCount);

		public:
			Application(const ApplicationDescriptor& applicationDescriptor);

			virtual ~Application();

			const Window& getWindow() const;

			float64 getCurrentTickTime() const;

			float64 getRendersPerSecondAverage() const;

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

			void tick();

			void update(float64 deltaTime);

			void preRender();

			void render(float64 remainingFrameTime);

			void postRender();

			void onEvent(Event& event);

			void onWindowResize(WindowResizeEvent& event);

			void onWindowClose(WindowCloseEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
			static constexpr float64 RENDERS_PER_SECOND = 165.0;

		private:
			struct TestSharedMemoryData {
				uint8 instanceCount;
			};

		private:
			static Application* instance;

		private:
			scope<Mutex> mutex;
			scope<SharedMemory> sharedMemory;
			TestSharedMemoryData* sharedMemoryData;

			// Need a scope because the window is created only if the property file is found.
			scope<Window> window;

			LayerStack layerStack;

			bool looping = true;
			bool needsResize = false;
			bool minimized = false;

			float64 fixedDeltaTime = 1.0;
			float64 previousTickTime = 0.0;
			float64 currentTickTime = 0.0;
			float64 timeAccumulator = 0.0;

			// Multiple of MAX_FRAMES_IN_FLIGHT.
			uint32 frameInFlightIndex = 0;
	};

	Application* createApplication();
}