#ifndef _TRAP_OPENGLTEXTURECUBE_H_
#define _TRAP_OPENGLTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class OpenGLTextureCube final : public TextureCube
	{
	public:
		OpenGLTextureCube(TextureParameters parameters);
		OpenGLTextureCube(std::string name, const std::array<std::string, 6>& filepaths, TextureParameters parameters);
		OpenGLTextureCube(std::string name, const std::string& filepath, InputFormat format, TextureParameters parameters);
		~OpenGLTextureCube();
		OpenGLTextureCube(const OpenGLTextureCube&) = delete;
		OpenGLTextureCube& operator=(const OpenGLTextureCube&) = delete;
		OpenGLTextureCube(OpenGLTextureCube&&) = default;
		OpenGLTextureCube& operator=(OpenGLTextureCube&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		uint32_t GetHandle() const;
		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;		

		InputFormat GetInputFormat() const override;
		std::array<Image*, 6> GetImages() const override;
		
	private:
		void LoadVerticalCross(const std::string& filepath);
		void LoadHorizontalCross(const std::string& filepath);
		void LoadFiles(const std::array<std::string, 6>& filepaths);
		
		uint32_t m_handle;
		std::string m_name;
		std::array<Scope<Image>, 6> m_images;
		TextureParameters m_parameters;
		InputFormat m_inputFormat;

		static uint32_t s_maxCombinedTextureUnits;
		static std::unordered_map<uint32_t, const OpenGLTextureCube*> s_boundCubeTextures;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::OpenGLTextureCube::GetHandle() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Graphics::API::OpenGLTextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::OpenGLTextureCube::GetImage()
{
	return m_images[0].get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureParameters TRAP::Graphics::API::OpenGLTextureCube::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::InputFormat TRAP::Graphics::API::OpenGLTextureCube::GetInputFormat() const
{
	return m_inputFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::array<TRAP::Image*, 6> TRAP::Graphics::API::OpenGLTextureCube::GetImages() const
{
	return { m_images[0].get(), m_images[1].get(), m_images[2].get(), m_images[3].get(), m_images[4].get(), m_images[5].get() };
}

#endif /*_TRAP_OPENGLTEXTURECUBE_H_*/