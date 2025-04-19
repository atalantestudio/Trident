#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Platform.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application& Application::get() {
		return *instance;
	}

	PropertyFile Application::loadPropertyFile() {
		const std::filesystem::path currentPath = Filesystem::getCurrentPath();
		const std::string currentDirectoryName = Filesystem::getCurrentDirectory().string();

		std::filesystem::path propertyFilePath = currentPath / currentDirectoryName;
		propertyFilePath += ".tpr";

		return Filesystem::load<PropertyFile>(propertyFilePath.string());
	}

	std::string Application::getWindowName(const std::string& projectName, uint8 instanceCount) {
		if (instanceCount == 0) {
			return projectName;
		}

		return Logger::format("[] ([])", projectName, static_cast<sint32>(instanceCount));
	}

	Application::Application(const ApplicationDescriptor& applicationDescriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(instance == nullptr, "Current instance already set.");

		instance = this;

		mutex = createScope<Mutex>("Mutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("SharedMemory", sizeof(TestSharedMemoryData));

		sharedMemoryData = static_cast<TestSharedMemoryData*>(sharedMemory->map(0, sizeof(TestSharedMemoryData)));

		constexpr uint8 MAX_PARALLEL_INSTANCES = 5;

		Platform::setWindowName(applicationDescriptor.projectName.data());

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			ENGINE_FATAL(EOVERFLOW, "More than [] instances running in parallel.", MAX_PARALLEL_INSTANCES);
		}

		PropertyFile propertyFile = loadPropertyFile();

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = getWindowName(applicationDescriptor.projectName, sharedMemoryData->instanceCount);
		windowDescriptor.width = propertyFile.getProperty<uint32>("ExtentWidth");
		windowDescriptor.height = propertyFile.getProperty<uint32>("ExtentHeight");

		sharedMemoryData->instanceCount += 1;

		ENGINE_TRACE("Running new instance (count = []).", static_cast<sint32>(sharedMemoryData->instanceCount));

		mutex->release();

		window = createScope<Window>(windowDescriptor);
		window->setEventListener<WindowCloseEvent>(BIND(Application::onWindowClose));

		const GraphicsAPI graphicsApi = propertyFile.getProperty<GraphicsAPI>("GraphicsAPI");

		Renderer::recreate(*window, graphicsApi);

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.loadOperation = LoadOperation::CLEAR;
		renderPassDescriptor.storeOperation = StoreOperation::STORE;

		renderPass = Renderer::createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;
		swapchainDescriptor.extentWidth = window->getSize()[0];
		swapchainDescriptor.extentHeight = window->getSize()[1];

		swapchain = Renderer::getBackend()->createSwapchain(swapchainDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		commandEncoder = Renderer::getDevice()->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		for (uint32 frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex += 1) {
			availableImageSemaphores[frameIndex] = Renderer::createSemaphore();
			finishedRenderSemaphores[frameIndex] = Renderer::createSemaphore();
			flightFences[frameIndex] = Renderer::createFence();
		}

		/* Constants */

		// DEFAULT_PROPERTY_FILE_PATH = Property file path used by default
		// API_QUEUE = Graphics API listed by priority.
		//             First APIs are more recent but more performant,
		//             last APIs are older but have better support

		/* Startup */

		// Open property file at DEFAULT_PROPERTY_FILE_PATH
			// If no file
				// Emit fatal
				// Return

		// Define int selectedApiIndex

		// Get "GraphicsAPI" setting (unsigned byte) from file
			// If no setting
				// Set selectedApiIndex = 0
			// Else if the setting is not in API_QUEUE
				// Emit warn "Unsupported graphics API. Using API 0. Updated the property in the file to avoid future warnings."
				// Set selectedApiIndex = 0
				// Overwrite file setting with selectedApiIndex
			// Else
				// Set selectedApiIndex = setting

		// Define queue apiTestQueue = API_QUEUE
			// Move the API at selectedApiIndex to the top

		// Define selectedApi
		// Define supportedApiIndices

		// For each API testedApi in API_QUEUE
			// Define available = testIfAvailable(testedApi)
			// If available
				// Push index of testedApi to supportedApiIndices

		// Define finalApiIndex = -1

		// For each API supportedApi in supportedApiIndices
			// If selectedApiIndex = index of supportedApi
				// Set finalApiIndex = index of supportedApi
				// Break

		// If finalApiIndex = -1
			// Emit warn "Selected API is unsupported"
			// Set finalApiIndex = 0

		// Emit info "Using backend"
		// Define selectedApi = API_QUEUE[supportedApiIndices[finalApiIndex]]

		// Initialize selectedApi

		/*int apiIndex = 0;
		int apiTestQueue[5] = {
			77, // Vulkan
			63, // DirectX
			48, // Metal
			22, // WebGPU
			19, // OpenGL
		};

		{
			const auto api = apiTestQueue[apiIndex];

			for (int i = apiIndex - 1; i >= 0; i--) {
				apiTestQueue[i + 1] = apiTestQueue[i];
			}

			apiTestQueue[0] = api;
		}*/
	}

	Application::~Application() {
		mutex->acquire();

		sharedMemoryData->instanceCount -= 1;

		ENGINE_DEBUG("Killing instance (count = []).", static_cast<sint32>(sharedMemoryData->instanceCount));

		sharedMemory->unmap(sharedMemoryData);
	}

	const Window& Application::getWindow() const {
		return *window;
	}

	WindowOld& Application::getWindowOld() {
		return *windowOld;
	}

	const ref<RenderPass>& Application::getRenderPass() const {
		return renderPass;
	}

	const ref<Swapchain>& Application::getSwapchain() const {
		return swapchain;
	}

	void Application::loop() {
		previousTickTime = Platform::getTime();

		while (looping) {
			tick();
		}
	}

	void Application::tick() {
		PROFILE_SCOPE("Application tick");

		window->processMessages();

		const float64 time = Platform::getTime();
		float64 currentTickTime = time - previousTickTime;

		previousTickTime = time;
		timeAccumulator += currentTickTime;

		while (timeAccumulator >= fixedDeltaTime) {
			// Integrate with a fixed time step.
			integrate(fixedDeltaTime);

			// Extract 1 * base delta time from the accumulator.
			// What's left is passed on to the next frame.
			timeAccumulator -= fixedDeltaTime;
		}

		// TODO: Interpolate between current time and timeAccumulator.
		if (!window->isMinimized()) {
			render(timeAccumulator);

			frameInFlightIndex = (frameInFlightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
		}
	}

	void Application::integrate(float64 fixedDeltaTime) {
		PROFILE_SCOPE("Application integrate (update)");

		for (const ref<Layer>& layer : layerStack) {
			layer->update(fixedDeltaTime);
		}
	}

	void Application::render(float64 remainingFrameTime) {
		PROFILE_SCOPE("Application render");

		const ref<Device> device = Renderer::getDevice();

		device->waitForFence(flightFences[frameInFlightIndex]);

		uint32 nextImageIndex;

		const Status acquireNextImageStatus = device->acquireNextImage(Application::get().getSwapchain(), availableImageSemaphores[frameInFlightIndex], nextImageIndex);

		ENGINE_ASSERT(acquireNextImageStatus == Status::SUBOPTIMAL || acquireNextImageStatus == Status::SUCCESS, "Failed to acquire next image.");

		device->resetFence(flightFences[frameInFlightIndex]);

		commandEncoder->reset();
		commandEncoder->begin();

		for (const ref<Layer>& layer : layerStack) {
			layer->render(remainingFrameTime, commandEncoder, nextImageIndex);
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, availableImageSemaphores[frameInFlightIndex], finishedRenderSemaphores[frameInFlightIndex], flightFences[frameInFlightIndex]);

		const Status presentStatus = device->getPresentQueue()->present(Application::get().getSwapchain(), finishedRenderSemaphores[frameInFlightIndex], nextImageIndex);

		if (presentStatus == Status::SUBOPTIMAL || presentStatus == Status::ERROR_OUT_OF_DATE || needsResize) {
			// TODO: Fix resize.
			resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == Status::SUCCESS, "Unhandled present status.");

		device->waitIdle();
	}

	void Application::resize() {
		sint32 extentWidth;
		sint32 extentHeight;

		Renderer::getWindowExtent(extentWidth, extentHeight);

		const ref<Device> device = Renderer::getDevice();

		device->waitIdle();

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = Application::get().getRenderPass();

		// NOTE: Beware of sint32 -> uint32 cast.
		swapchainDescriptor.extentWidth = extentWidth;
		swapchainDescriptor.extentHeight = extentHeight;

		Application::get().getSwapchain()->recreate(swapchainDescriptor);

		needsResize = false;
	}

	void Application::close() {
		looping = false;
	}

	void Application::onEvent(Event& event) {
		/*PROFILE_SCOPE(FUNC_SIG);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowCloseEvent>(BIND(Application::onWindowClose));

		for (chain<Layer*>::const_reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
			if (event.handled) {
				break;
			}

			(*i)->onEvent(event);
		}*/
	}

	void Application::onWindowClose(WindowCloseEvent& event) {
		close();
	}
}