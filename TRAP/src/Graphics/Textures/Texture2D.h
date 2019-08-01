#ifndef _TRAP_TEXTURE2D_H_
#define _TRAP_TEXTURE2D_H_

#include "Texture.h"

namespace TRAP::Graphics
{
	class Texture2D : public Texture
	{
	public:
		static std::unique_ptr<Texture2D> CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters = TextureParameters());
		static std::unique_ptr<Texture2D> Create(TextureParameters parameters = TextureParameters());

		TextureType GetType() const override;
	};
}

inline TRAP::Graphics::TextureType TRAP::Graphics::Texture2D::GetType() const
{
	return TextureType::Texture2D;
}

#endif /*_TRAP_TEXTURE2D_H_*/