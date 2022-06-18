#ifndef _TRAP_VULKANTEXTURECUBE_H_
#define _TRAP_VULKANTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class VulkanTextureCube final : public TextureCube
	{
	public:
		explicit VulkanTextureCube(TextureParameters parameters);
		VulkanTextureCube(std::string name, const std::array<std::string_view, 6>& filepaths, TextureParameters parameters);
		VulkanTextureCube(std::string name, std::string_view filepath, InputFormat format, TextureParameters parameters);
		VulkanTextureCube(std::string name, const Scope<Image>& img, InputFormat format, TextureParameters parameters);
		~VulkanTextureCube();
		VulkanTextureCube(const VulkanTextureCube&) = default;
		VulkanTextureCube& operator=(const VulkanTextureCube&) = default;
		VulkanTextureCube(VulkanTextureCube&&) = default;
		VulkanTextureCube& operator=(VulkanTextureCube&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;
	};
}

#endif /*_TRAP_VULKANTEXTURECUBE_H_*/