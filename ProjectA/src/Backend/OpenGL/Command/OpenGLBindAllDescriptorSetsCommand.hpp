#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

#include "Engine/Renderer/DescriptorSetLayout.hpp"

namespace ProjectA {
	class OpenGLBindAllDescriptorSetsCommand : public OpenGLCommand {
		public:
			OpenGLBindAllDescriptorSetsCommand(const chain<DescriptorSetLayout>& descriptorSetLayouts);

			void execute() const;

		private:
			const chain<DescriptorSetLayout>& descriptorSetLayouts;
	};
}