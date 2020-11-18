#ifndef _TRAP_TEXTURECUBE_H_
#define _TRAP_TEXTURECUBE_H_

#include "Texture.h"

namespace TRAP::Graphics
{
	enum class InputFormat
	{
		NONE = 0,
		Vertical_Cross,
		Horizontal_Cross,
	};

	class TextureCube : public Texture
	{
	public:
		static Scope<TextureCube> CreateFromFiles(const std::string& name, const std::array<std::string_view, 6>& filepaths, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> CreateFromCross(const std::string& name, std::string_view filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> CreateFromCross(std::string_view filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> CreateFromCrossImage(const std::string& name, const Scope<Image>& img, InputFormat format, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> Create(TextureParameters parameters = TextureParameters());

		InputFormat GetInputFormat() const;
		std::array<std::string, 6> GetFilePaths() const;

	protected:
		TextureCube();
		
		static uint32_t s_maxCubeTextureSize;

		std::array<std::string, 6> m_filepaths;
		InputFormat m_inputFormat;
	};
}

#endif