#pragma once

#include "Engine/ECS/ECS.hpp"
#include "Engine/GUI/GUISubcomponent.hpp"

namespace ProjectA {
	enum class LayoutDistribution : uint8 {
		HORIZONTAL = BIT(0),
		VERTICAL = BIT(1),
	};

	AS_FLAG(LayoutDistribution);

	enum class FillMode : uint8 {
		FIXED_HORIZONTAL = 0,
		FIXED_VERTICAL = 0,
		EXPAND_HORIZONTAL = BIT(0),
		EXPAND_VERTICAL = BIT(1),
	};

	AS_FLAG(FillMode);

	class GUIObject {
		public:
			GUIObject();

			void setLayoutDistribution(LayoutDistribution layoutDistribution);

			void setFillMode(FillMode fillMode);

			void setDefaultSize(const vec2<float32>& defaultSize);

			const vec2<float32>& getPosition() const;

			void setPosition(const vec2<float32>& position);

			void setMargin(const vec2<float32>& margin);

			void setSpacing(const vec2<float32>& spacing);

			void setColor(const vec4<float32>& color);

			uint32 getTextureIndex() const;

			void setTextureIndex(uint32 textureIndex);

			const chain<GUISubcomponent>& getSubcomponents() const;

			template<typename T, typename... U>
			void append(U&&... arguments) {
				const ref<T> object = createRef<T>(std::forward<U>(arguments)...);

				objects.emplace_back(object);
			}

			virtual void compute(ECS& ecs, const vec2<float32>& parentSize, const vec2<float32>& parentMargin, chain<vec4<float32>>& colors, const ref<Font>& font);

		public:
			LayoutDistribution layoutDistribution = LayoutDistribution::HORIZONTAL;
			FillMode fillMode = FillMode::FIXED_HORIZONTAL | FillMode::FIXED_VERTICAL;
			vec2<float32> position = {0.0f, 0.0f};
			vec2<float32> defaultSize = {0.0f, 0.0f};
			vec2<float32> margin = {0.0f, 0.0f};
			vec2<float32> spacing = {0.0f, 0.0f};
			vec4<float32> color = {0.0f, 0.0f, 0.0f, 0.0f};
			uint32 textureIndex = 0;
			chain<ref<GUIObject>> objects;
			chain<GUISubcomponent> subcomponents;
	};
}