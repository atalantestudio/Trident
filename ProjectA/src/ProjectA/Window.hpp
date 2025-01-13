#pragma once

#include "ProjectA/core.hpp"
#include "ProjectA/Event/Event.hpp"

namespace ProjectA {
	struct WindowData {
		std::string title;
		unsigned int width, height;

		WindowData(const std::string& title = "Project A", unsigned int width = 1280, unsigned int height = 720): title(title), width(width), height(height) {}
	};

	class Window {
		public:
			static Scope<Window> create(const WindowData& data = WindowData());

		public:
			using EventCallback = std::function<void(Event&)>;

			virtual ~Window() = default;

			virtual void* getNativeWindow() const = 0;
			virtual unsigned int getWidth() const = 0;
			virtual unsigned int getHeight() const = 0;
			virtual bool isVSyncEnabled() const = 0;
			virtual void setEventCallback(const EventCallback& callback) = 0;
			virtual void setVSyncState(bool state) = 0;
			virtual void update() = 0;
	};
}