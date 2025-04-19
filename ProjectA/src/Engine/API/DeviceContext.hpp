#pragma once

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class DeviceContext {
		public:
			DeviceContext(const Window& window);

			~DeviceContext();

			void* getProcedureAddress(std::string_view name) const;

			void swapBuffers() const;

		private:
			void* data;
	};
}