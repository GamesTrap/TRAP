#ifndef _TRAP_OPENGLTEXTURECUBE_H_
#define _TRAP_OPENGLTEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class OpenGLTextureCube final : public TextureCube
	{
	public:
		explicit OpenGLTextureCube(TextureParameters parameters);
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
		std::string GetFilePath() const override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;		

		InputFormat GetInputFormat() const override;
		std::array<std::string, 6> GetFilePaths() const override;
		
	private:
		
		void LoadVerticalCross();
		void LoadHorizontalCross();
		void LoadFiles();

		bool CheckImageSize(const Scope<Image>& image) const;
		void InitializeTexture();

		uint32_t m_handle;
		std::string m_name;
		std::array<std::string, 6> m_filePaths;
		TextureParameters m_parameters;
		InputFormat m_inputFormat;

		static uint32_t s_maxCombinedTextureUnits;
		static std::unordered_map<uint32_t, const OpenGLTextureCube*> s_boundCubeTextures;
	};
}

#endif /*_TRAP_OPENGLTEXTURECUBE_H_*/