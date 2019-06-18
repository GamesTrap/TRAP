#ifndef _TRAP_D3D12TEXTURE2D_H_
#define _TRAP_D3D12TEXTURE2D_H_

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12Texture2D : public Texture2D
	{
	public:
		explicit D3D12Texture2D(TextureParameters parameters = TextureParameters());
		D3D12Texture2D(std::string name, const std::string& filepath, TextureParameters parameters = TextureParameters());
		~D3D12Texture2D();
		D3D12Texture2D(const D3D12Texture2D&) = delete;
		D3D12Texture2D& operator=(const D3D12Texture2D&) = delete;
		D3D12Texture2D(D3D12Texture2D&&) = default;
		D3D12Texture2D& operator=(D3D12Texture2D&&) = default;

		void Bind(unsigned slot) const override;
		void Unbind(unsigned slot) const override;

		const std::string& GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

	private:
		std::string m_name;
		std::unique_ptr<Image> m_image;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Texture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::D3D12Texture2D::GetImage()
{
	return m_image.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::TextureParameters TRAP::Graphics::API::D3D12Texture2D::GetParameters()
{
	return TextureParameters();
}

#endif

#endif /*_TRAP_D3D12TEXTURE2D_H_*/