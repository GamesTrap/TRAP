#ifndef TRAP_RENDERTARGET_H
#define TRAP_RENDERTARGET_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Texture;

	class RenderTarget
	{
	public:
		/// @brief Create a new render target from the given description.
		/// @param desc Render target description.
		/// @return Created render target.
		[[nodiscard]] static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);

		/// @brief Destructor.
		virtual ~RenderTarget();

		/// @brief Copy constructor.
		constexpr RenderTarget(const RenderTarget&) = delete;
		/// @brief Copy assignment operator.
		constexpr RenderTarget& operator=(const RenderTarget&) = delete;
		/// @brief Move constructor.
		RenderTarget(RenderTarget&&) noexcept = default;
		/// @brief Move assignment operator.
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

		/// @brief Retrieve the width of the render target.
		/// @return Render target width.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the height of the render target.
		/// @return Render target height.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// @brief Retrieve the depth of the render target.
		/// @return Render target depth.
		[[nodiscard]] constexpr u32 GetDepth() const noexcept;
		/// @brief Retrieve the array size of the render target.
		/// @return Render target array size.
		[[nodiscard]] constexpr u32 GetArraySize() const noexcept;
		/// @brief Retrieve the mip levels of the render target.
		/// @return Render target mip levels.
		[[nodiscard]] constexpr u32 GetMipLevels() const noexcept;
		/// @brief Retrieve the sample count of the render target.
		/// @return Render target sample count.
		[[nodiscard]] constexpr RendererAPI::SampleCount GetSampleCount() const noexcept;
		/// @brief Retrieve the sample quality of the render target.
		/// @return Render target sample quality.
		[[nodiscard]] constexpr u32 GetSampleQuality() const noexcept;
		/// @brief Retrieve the image format of the render target.
		/// @return Render target image format.
		[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat GetImageFormat() const noexcept;
		/// @brief Retrieve the clear color of the render target.
		/// @return Render target clear color.
		[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ClearValue GetClearValue() const noexcept;
		/// @brief Retrieve the descriptor type of the render target.
		/// @return Render target descriptor type.
		[[nodiscard]] constexpr RendererAPI::DescriptorType GetDescriptorType() const noexcept;

		/// @brief Retrieve the texture base of the render target.
		/// @return Render target texture base.
		[[nodiscard]] Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

	protected:
		/// @brief Constructor.
		explicit RenderTarget(const RendererAPI::RenderTargetDesc& desc);

		TRAP::Ref<TRAP::Graphics::Texture> m_texture = nullptr;

		RendererAPI::ClearValue m_clearValue = RendererAPI::Color{1.0, 1.0, 1.0, 1.0};
		u32 m_arraySize = 0;
		u32 m_depth = 0;
		u32 m_width = 0;
		u32 m_height = 0;
		RendererAPI::DescriptorType m_descriptors = RendererAPI::DescriptorType::Undefined;
		u32 m_mipLevels = 0;
		u32 m_sampleQuality = 0;
		TRAP::Graphics::API::ImageFormat m_format = TRAP::Graphics::API::ImageFormat::Undefined;
		RendererAPI::SampleCount m_sampleCount = RendererAPI::SampleCount::One;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetDepth() const noexcept
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetArraySize() const noexcept
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetMipLevels() const noexcept
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const noexcept
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::RenderTarget::GetSampleQuality() const noexcept
{
	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const noexcept
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ClearValue TRAP::Graphics::RenderTarget::GetClearValue() const noexcept
{
	return m_clearValue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const noexcept
{
	return m_descriptors;
}

#endif /*TRAP_RENDERTARGET_H*/