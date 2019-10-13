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
		static Scope<TextureCube> CreateFromFiles(const std::string& name, const std::array<std::string, 6>& filepaths, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> CreateFromCross(const std::string& name, const std::string& filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> CreateFromCross(const std::string& filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static Scope<TextureCube> Create(TextureParameters parameters = TextureParameters());

		TextureType GetType() const override;
		virtual InputFormat GetInputFormat() const = 0;
		virtual std::array<std::string, 6> GetFilePaths() const = 0;

	protected:
		static uint32_t s_maxCubeTextureSize;
	};
}

#endif