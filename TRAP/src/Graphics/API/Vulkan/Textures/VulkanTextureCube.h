#ifndef _TRAP_VULKANTEXTURECUBE_H_
#define _TRAP_VULKANTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class VulkanTextureCube final : public TextureCube
	{
	public:
		VulkanTextureCube(TextureParameters parameters);
		VulkanTextureCube(std::string name, const std::array<std::string, 6> & filepaths, TextureParameters parameters);
		VulkanTextureCube(std::string name, std::string filepath, InputFormat format, TextureParameters parameters);
		~VulkanTextureCube();
		VulkanTextureCube(const VulkanTextureCube&) = default;
		VulkanTextureCube& operator=(const VulkanTextureCube&) = default;
		VulkanTextureCube(VulkanTextureCube&&) = default;
		VulkanTextureCube& operator=(VulkanTextureCube&&) = default;

		void Bind(unsigned int slot) const override;
		void Unbind(unsigned int slot) const override;

		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

		InputFormat GetInputFormat() const override;
		std::array<Image*, 6> GetImages() const override;

	private:
		std::string m_name;
		std::vector<std::unique_ptr<Image>> m_images;
		TextureParameters m_parameters;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Graphics::API::VulkanTextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::VulkanTextureCube::GetImage()
{
	return m_images[0].get(); //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTextureCube::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::array<TRAP::Image*, 6> TRAP::Graphics::API::VulkanTextureCube::GetImages() const
{
	return { m_images[0].get(), m_images[1].get(), m_images[2].get(), m_images[3].get(), m_images[4].get(), m_images[5].get(), };
}

#endif /*_TRAP_VULKANTEXTURECUBE_H_*/