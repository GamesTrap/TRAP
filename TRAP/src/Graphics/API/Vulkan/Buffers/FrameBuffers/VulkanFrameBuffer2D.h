#ifndef _TRAP_VULKANFRAMEBUFFER2D_H_
#define _TRAP_VULKANFRAMEBUFFER2D_H_

#include "Graphics/Buffers/FrameBuffers/FrameBuffer2D.h"
#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class VulkanFrameBuffer2D final : public FrameBuffer2D
	{
	public:
		VulkanFrameBuffer2D(uint32_t width, uint32_t height);
		virtual ~VulkanFrameBuffer2D();
		VulkanFrameBuffer2D(const VulkanFrameBuffer2D&) = delete;
		VulkanFrameBuffer2D& operator=(const VulkanFrameBuffer2D&) = delete;
		VulkanFrameBuffer2D(VulkanFrameBuffer2D&&) = default;
		VulkanFrameBuffer2D& operator=(VulkanFrameBuffer2D&&) = default;

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		Texture* GetTexture() const override;
		void SetClearColor(const Math::Vec4& color) override;

	private:
		void Init() override;

		uint32_t m_frameBufferHandle;
		uint32_t m_depthBufferHandle;

		uint32_t m_width, m_height;
		Math::Vec4 m_clearColor;
		std::unique_ptr<Texture2D> m_texture;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::VulkanFrameBuffer2D::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::VulkanFrameBuffer2D::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::Texture* TRAP::Graphics::API::VulkanFrameBuffer2D::GetTexture() const
{
	return m_texture.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::VulkanFrameBuffer2D::SetClearColor(const Math::Vec4& color)
{
	m_clearColor = color;
}

#endif /*_TRAP_VULKANFRAMEBUFFER2D_H_*/