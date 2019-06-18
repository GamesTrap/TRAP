#ifndef _TRAP_VULKANTEXTURE2D_H_
#define _TRAP_VULKANTEXTURE2D_H_

namespace TRAP::Graphics::API
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		explicit VulkanTexture2D(TextureParameters parameters = TextureParameters());
		VulkanTexture2D(std::string name, const std::string& filepath, TextureParameters parameters = TextureParameters());
		~VulkanTexture2D();
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;
		VulkanTexture2D(VulkanTexture2D&&) = default;
		VulkanTexture2D& operator=(VulkanTexture2D&&) = default;

		void Bind(unsigned slot) const override;
		void Unbind(unsigned slot) const override;

		const std::string& GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

	private:
		std::string m_name;
		std::unique_ptr<Image> m_image;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanTexture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::VulkanTexture2D::GetImage()
{
	return m_image.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::TextureParameters TRAP::Graphics::API::VulkanTexture2D::GetParameters()
{
	return TextureParameters();
}

#endif /*_TRAP_VULKANTEXTURE2D_H_*/