#pragma once

#include "Engine/base.hpp"
#include "Engine/Event/Event.hpp"

namespace ProjectA {
	struct WindowData {
		std::string title;
		// TODO: Use vec2<sint32>
		sint32 width;
		sint32 height;

		WindowData(const std::string& title = "Project A", sint32 width = 1280, sint32 height = 720) :
			title(title),
			width(width),
			height(height) {}
	};

	// TODO: OpenGLWindow and VulkanWindow.
	class Window {
		public:
			static scope<Window> create(const WindowData& data = WindowData());

		public:
			using EventCallback = std::function<void(Event&)>;

			virtual ~Window() = default;

			virtual void* getNativeWindow() const = 0;
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;
			virtual bool isVSyncEnabled() const = 0;
			virtual void setEventCallback(const EventCallback& callback) = 0;
			virtual void setVSyncState(bool state) = 0;
			virtual void update() = 0;
	};
}