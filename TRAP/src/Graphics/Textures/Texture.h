#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	enum class TextureWrap
	{
		NONE = 0,
		Repeat,
		Mirrored_Repeat,
		Clamp_To_Edge,
		Clamp_To_Border
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class TextureType
	{
		NONE = 0,
		Texture2D
	};

	struct TextureParameters
	{
		TextureFilter Filter;
		TextureWrap Wrap;

		TextureParameters()
			: Filter(TextureFilter::Nearest), Wrap(TextureWrap::Clamp_To_Edge)
		{			
		}
		explicit TextureParameters(const TextureFilter filter)
			: Filter(filter), Wrap(TextureWrap::Clamp_To_Edge)
		{			
		}
		TextureParameters(const TextureFilter filter, const TextureWrap wrap)
			: Filter(filter), Wrap(wrap)
		{			
		}
	};

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		virtual void Bind(unsigned int slot = 0) const = 0;
		virtual void Unbind(unsigned int slot = 0) const = 0;

		virtual std::string GetName() const = 0;
		virtual TextureType GetType() const = 0;
		virtual Image* GetImage() = 0;
		virtual TextureParameters GetParameters() = 0;

		virtual void SetWrap(TextureWrap wrap);
		virtual void SetFilter(TextureFilter filter);

		static uint8_t GetStrideFromFormat(ImageFormat format);

	protected:
		static TextureWrap s_WrapMode;
		static TextureFilter s_FilterMode;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Texture::SetWrap(const TextureWrap wrap)
{
	s_WrapMode = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Texture::SetFilter(const TextureFilter filter)
{
	s_FilterMode = filter;
}

#endif /*_TRAP_TEXTURE_H_*/