#include "TRAPPCH.h"
#include "TextureManager.h"

std::vector<std::unique_ptr<TRAP::Graphics::API::Texture>> TRAP::Graphics::TextureManager::s_textures;

void TRAP::Graphics::TextureManager::Add(std::unique_ptr<API::Texture> texture)
{
	s_textures.push_back(std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Texture* TRAP::Graphics::TextureManager::Get(const std::string& name)
{
	for (const auto& texture : s_textures)
		if (texture->GetName() == name)
			return texture.get();

	return Get("Fallback");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	s_textures.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Reload(const std::string& nameOrVirtualPath)
{
	//Name
	if (nameOrVirtualPath[0] != '/')
	{
		for (auto& texture : s_textures)
		{
			if (texture->GetName() == nameOrVirtualPath)
			{
				const std::string filePath = texture->GetImage()->GetFilePath();
				if (filePath.empty())
				{
					TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
					return;
				}

				const std::string name = texture->GetName();
				const API::TextureType textureType = texture->GetType();
				const API::TextureParameters parameters = texture->GetParameters();

				if (textureType == API::TextureType::Texture2D)
				{
					texture.reset();
					texture = API::Texture2D::CreateFromFile(name, filePath, parameters);
					TP_INFO("[TextureManager] Reloaded: \"", nameOrVirtualPath, "\"");
				}
				else
					TP_WARN("[TextureManager] Unknown TextureType: \"", nameOrVirtualPath, "\"");

				return;
			}
		}
	}
	else //Virtual Path
	{
		for (const auto& texture : s_textures)
			if (nameOrVirtualPath == texture->GetImage()->GetFilePath())
			{
				Reload(texture);
				return;
			}
		TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Reload(const std::unique_ptr<API::Texture>& texture)
{
	for (auto& s_Texture : s_textures)
	{
		if (s_Texture == texture)
		{
			const std::string name = texture->GetName();
			const std::string filePath = texture->GetImage()->GetFilePath();
			if (filePath.empty())
			{
				TP_WARN("[TextureManager] Could not find Texture: \"", name, "\" to reload.");
				return;
			}

			const API::TextureType textureType = texture->GetType();
			const API::TextureParameters parameters = texture->GetParameters();

			if (textureType == API::TextureType::Texture2D)
			{
				s_Texture.reset();
				s_Texture = API::Texture2D::CreateFromFile(name, filePath, parameters);
				TP_INFO("[TextureManager] Reloaded: \"", name, "\"");
			}
			else
				TP_WARN("[TextureManager] Unknown TextureType: \"", name, "\"");

			return;
		}
	}

	TP_WARN("[TextureManager] Could not find Shader: \"", texture->GetName(), "\" to reload.");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::ReloadAll()
{
	TP_INFO("[TextureManager] Reloading all may take a while...");
	for (auto& texture : s_textures)
		Reload(texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown()
{
	TP_DEBUG("[TextureManager] Destroying Textures");
	Clean();
}