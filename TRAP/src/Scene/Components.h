#ifndef _TRAP_COMPONENTS_H_
#define _TRAP_COMPONENTS_H_

#include "Maths/Math.h"

namespace TRAP
{
	struct TransformComponent
	{
		Math::Mat4 Transform{ 1.0f };

		TransformComponent() = default;
		~TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(TransformComponent&&) = default;
		TransformComponent& operator=(const TransformComponent&) = default;
		TransformComponent& operator=(TransformComponent&&) = default;
		explicit TransformComponent(const TRAP::Math::Mat4& transform)
			: Transform(transform)
		{}

		operator TRAP::Math::Mat4&()
		{
			return Transform;
		}

		operator const TRAP::Math::Mat4&() const
		{
			return Transform;
		}
	};

	struct SpriteRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		~SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(SpriteRendererComponent&&) = default;
		SpriteRendererComponent& operator=(const SpriteRendererComponent&) = default;
		SpriteRendererComponent& operator=(SpriteRendererComponent&&) = default;
		SpriteRendererComponent(const TRAP::Math::Vec4& color)
			: Color(color)
		{}
	};
}

#endif /*_TRAP_COMPONENTS_H_*/