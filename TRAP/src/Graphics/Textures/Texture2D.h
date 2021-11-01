#ifndef _TRAP_TEXTURE2D_H_
#define _TRAP_TEXTURE2D_H_

#include "Texture.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	class Texture2D : public Texture
	{
	public:
		static Scope<Texture2D> CreateFromFile(const std::string& name, std::string_view filepath);
		static Scope<Texture2D> CreateFromFile(std::string_view filepath);
		static Scope<Texture2D> CreateFromImage(const std::string& name, const TRAP::Scope<TRAP::Image>& img);
		static Scope<Texture2D> CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel,
		                                    Image::ColorFormat format);
		static Scope<Texture2D> Create(); //Fallback Texture

		uint32_t GetDepth() const override;
		uint32_t GetArraySize() const override;
		const std::string& GetFilePath() const;

		void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;

		~Texture2D() override = default;

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Texture2D(const Texture2D&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Texture2D& operator=(const Texture2D&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Texture2D(Texture2D&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Texture2D& operator=(Texture2D&&) = default;

	protected:
		Texture2D();
		Texture2D(const std::string& name, std::string_view filepath);

		std::string m_filepath;
	};
}

#endif /*_TRAP_TEXTURE2D_H_*/