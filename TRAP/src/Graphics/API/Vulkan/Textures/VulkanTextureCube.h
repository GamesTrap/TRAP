#ifndef _TRAP_VULKANTEXTURECUBE_H_
#define _TRAP_VULKANTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class VulkanTextureCube final : public TextureCube
	{
	public:
		explicit VulkanTextureCube(TextureParameters parameters);
		VulkanTextureCube(std::string name, const std::array<std::string, 6>& filepaths, TextureParameters parameters);
		VulkanTextureCube(std::string name, const std::string& filepath, InputFormat format, TextureParameters parameters);
		~VulkanTextureCube();
		VulkanTextureCube(const VulkanTextureCube&) = default;
		VulkanTextureCube& operator=(const VulkanTextureCube&) = default;
		VulkanTextureCube(VulkanTextureCube&&) = default;
		VulkanTextureCube& operator=(VulkanTextureCube&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		std::string GetName() const override;
		std::string GetFilePath() const override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

		InputFormat GetInputFormat() const override;
		std::array<std::string, 6> GetFilePaths() const override;

	private:
		std::string m_name;
		std::array<std::string, 6> m_filePaths;
		TextureParameters m_parameters;
	};
}

#endif /*_TRAP_VULKANTEXTURECUBE_H_*/