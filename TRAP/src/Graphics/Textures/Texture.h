#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

namespace TRAP::Graphics::API
{
	enum class TextureWrap
	{
		NONE = 0,
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TextureFilter
	{
		NONE = 0,
		LINEAR,
		NEAREST
	};

	enum class TextureFormat
	{
		NONE = 0,
		RGB,
		RGBA
	};

	enum class TextureType
	{
		NONE = 0,
		Texture2D
	};

	struct TextureParameters
	{
		TextureFormat Format;
		TextureFilter Filter;
		TextureWrap Wrap;

		TextureParameters()
			: Format(TextureFormat::RGBA), Filter(TextureFilter::NEAREST), Wrap(TextureWrap::CLAMP_TO_EDGE)
		{
		}
		TextureParameters(const TextureFormat format, const TextureFilter filter, const TextureWrap wrap)
			: Format(format), Filter(filter), Wrap(wrap)
		{			
		}
		explicit TextureParameters(const TextureFilter filter)
			: Format(TextureFormat::RGBA), Filter(filter), Wrap(TextureWrap::CLAMP_TO_EDGE)
		{			
		}
		TextureParameters(const TextureFilter filter, const TextureWrap wrap)
			: Format(TextureFormat::RGBA), Filter(filter), Wrap(wrap)
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

		virtual const std::string& GetName() const = 0;
		virtual TextureType GetType() const = 0;
		virtual Image* GetImage() = 0;
		virtual TextureParameters GetParameters() = 0;

		static void SetWrap(TextureWrap mode);
		static void SetFilter(TextureFilter mode);

		static uint8_t GetStrideFromFormat(TextureFormat format);

	protected:
		static TextureWrap s_WrapMode;
		static TextureFilter s_FilterMode;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Texture::SetWrap(const TextureWrap mode)
{
	s_WrapMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Texture::SetFilter(const TextureFilter mode)
{
	s_FilterMode = mode;
}

#endif /*_TRAP_TEXTURE_H_*/