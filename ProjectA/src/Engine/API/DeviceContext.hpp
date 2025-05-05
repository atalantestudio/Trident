#pragma once

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	#define LOAD_PROCEDURE_ADDRESS(deviceContext, procedure) procedure = static_cast<decltype(procedure)>(deviceContext.getProcedureAddress(#procedure))

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