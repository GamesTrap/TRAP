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

		void Bind(unsigned int slot = 0) const override;
		void Unbind(unsigned int slot = 0) const override;

		uint32_t GetHandle() const;
		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;		

	private:
		void Load(const std::string& filepath);

		std::unique_ptr<Image> m_image;
		std::string m_name;
		TextureParameters m_parameters;
		uint32_t m_handle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::OpenGLTexture2D::GetHandle() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Graphics::API::OpenGLTexture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::OpenGLTexture2D::GetImage()
{
	return m_image.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureParameters TRAP::Graphics::API::OpenGLTexture2D::GetParameters()
{
	return m_parameters;
}

#endif /*_TRAP_OPENGLTEXTURE2D_H_*/