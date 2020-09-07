#ifndef _TRAP_OPENGLTEXTURECUBE_H_
#define _TRAP_OPENGLTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class OpenGLTextureCube final : public TextureCube
	{
	public:
		explicit OpenGLTextureCube(TextureParameters parameters);
		OpenGLTextureCube(std::string name, const std::array<std::string_view, 6>& filepaths, TextureParameters parameters);
		OpenGLTextureCube(std::string name, std::string_view filepath, InputFormat format, TextureParameters parameters);
		OpenGLTextureCube(std::string name, const Scope<Image>& img, InputFormat format, TextureParameters parameters);
		~OpenGLTextureCube();
		OpenGLTextureCube(const OpenGLTextureCube&) = delete;
		OpenGLTextureCube& operator=(const OpenGLTextureCube&) = delete;
		OpenGLTextureCube(OpenGLTextureCube&&) = default;
		OpenGLTextureCube& operator=(OpenGLTextureCube&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		uint32_t GetHandle() const;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;
		
	private:
		void SetupVerticalCross();
		void SetupHorizontalCross();
		void LoadVerticalCross(const Scope<Image>& img);
		void LoadHorizontalCross(const Scope<Image>& img);
		void LoadFiles();

		bool CheckImageSize(const Scope<Image>& image) const;
		void InitializeTexture();

		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLTEXTURECUBE_H_*/