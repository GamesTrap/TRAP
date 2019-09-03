#ifndef _TRAP_VULKANTEXTURE2D_H_
#define _TRAP_VULKANTEXTURE2D_H_

#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class VulkanTexture2D final : public Graphics::Texture2D
	{
	public:
		explicit VulkanTexture2D(TextureParameters parameters);
		VulkanTexture2D(ImageFormat format, uint32_t width, uint32_t height, TextureParameters parameters);
		VulkanTexture2D(std::string name, const std::string& filepath, TextureParameters parameters);
		~VulkanTexture2D();
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;
		VulkanTexture2D(VulkanTexture2D&&) = default;
		VulkanTexture2D& operator=(VulkanTexture2D&&) = default;

		void Bind(unsigned int slot = 0) const override;
		void Unbind(unsigned int slot = 0) const override;

		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

	private:
		std::unique_ptr<Image> m_image;
		std::string m_name;
		TextureParameters m_parameters;
		uint32_t m_handle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Graphics::API::VulkanTexture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::VulkanTexture2D::GetImage()
{
	return m_image.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTexture2D::GetParameters()
{
	return m_parameters;
}

#endif /*_TRAP_VULKANTEXTURE2D_H_*/