#ifndef _TRAP_VULKANTEXTURE2D_H_
#define _TRAP_VULKANTEXTURE2D_H_

#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class VulkanTexture2D final : public Texture2D
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

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const override;

		std::string GetName() const override;
		std::string GetFilePath() const override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

	private:
		std::string m_name;
		std::string m_filePath;
		TextureParameters m_parameters;
	};
}

#endif /*_TRAP_VULKANTEXTURE2D_H_*/