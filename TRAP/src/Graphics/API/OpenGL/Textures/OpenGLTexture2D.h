#ifndef _TRAP_OPENGLTEXTURE2D_H_
#define _TRAP_OPENGLTEXTURE2D_H_

#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class OpenGLTexture2D final : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(TextureParameters parameters);
		OpenGLTexture2D(ImageFormat format, uint32_t width, uint32_t height, TextureParameters parameters);
		OpenGLTexture2D(std::string name, const std::string& filepath, TextureParameters parameters);
		~OpenGLTexture2D();
		OpenGLTexture2D(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D& operator=(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D(OpenGLTexture2D&&) = default;
		OpenGLTexture2D& operator=(OpenGLTexture2D&&) = default;

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const override;

		uint32_t GetHandle() const;
		std::string GetName() const override;
		std::string GetFilePath() const override;
		//Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;		

	private:
		void Load();

		std::string m_filePath;
		//Scope<Image> m_image;
		std::string m_name;
		TextureParameters m_parameters;
		uint32_t m_handle;

		static uint32_t s_maxCombinedTextureUnits;
		static std::unordered_map<uint32_t, const OpenGLTexture2D*> s_bound2DTextures;
	};
}

#endif /*_TRAP_OPENGLTEXTURE2D_H_*/