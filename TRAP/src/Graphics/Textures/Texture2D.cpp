#include "TRAPPCH.h"
#include "Texture2D.h"

std::unique_ptr<TRAP::Graphics::API::Texture2D> TRAP::Graphics::API::Texture2D::Create(TextureParameters parameters)
{
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::D3D12:
		return std::make_unique<D3D12Texture2D>(parameters);

	case RenderAPI::VULKAN:
		return std::make_unique<VulkanTexture2D>(parameters);

	case RenderAPI::OPENGL:
		return std::make_unique<OpenGLTexture2D>(parameters);

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Texture2D> TRAP::Graphics::API::Texture2D::CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters)
{
	switch (Context::GetRenderAPI())
	{
	case RenderAPI::D3D12:
		return std::make_unique<D3D12Texture2D>(name, filepath, parameters);

	case RenderAPI::VULKAN:
		return std::make_unique<VulkanTexture2D>(name, filepath, parameters);

	case RenderAPI::OPENGL:
		return std::make_unique<OpenGLTexture2D>(name, filepath, parameters);

	default:
		return nullptr;
	}
}