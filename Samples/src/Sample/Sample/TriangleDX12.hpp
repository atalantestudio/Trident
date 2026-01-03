#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class SampleRenderer : public Renderer {
		private:
			static constexpr view<char8> shaderCode = R"(
				struct VertexOutput {
					float4 position : SV_POSITION;
					float4 color : COLOR;
				};

				VertexOutput vertex(float4 position : POSITION, float4 color : COLOR) {
					VertexOutput output;
					output.position = position;
					output.color = color;

					return output;
				}

				float4 fragment(VertexOutput input) : SV_TARGET {
					return input.color;
				}
			)";

		public:
			void build() override {
				PipelineLayoutDescriptor pipelineLayoutDescriptor{};

				const ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

				ShaderModuleDescriptor vertexShaderModuleDescriptor{};
				vertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
				vertexShaderModuleDescriptor.entryPoint = "vertex";

				ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
				fragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
				fragmentShaderModuleDescriptor.entryPoint = "fragment";

				const ref<ShaderModule> vertexShaderModule = loadHlslShaderModule(shaderCode, vertexShaderModuleDescriptor);
				const ref<ShaderModule> fragmentShaderModule = loadHlslShaderModule(shaderCode, fragmentShaderModuleDescriptor);

				// TODO
				BREAKPOINT();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {}

			void resize(WindowResizeEvent& event) override {}
	};
}