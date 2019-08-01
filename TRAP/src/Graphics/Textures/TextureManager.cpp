#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture2D.h"

std::vector<std::unique_ptr<TRAP::Graphics::Texture>> TRAP::Graphics::TextureManager::s_textures;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(std::unique_ptr<Texture> texture)
{
	s_textures.push_back(std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Get(const std::string& name)
{
	for(const auto& texture : s_textures)
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
				const TextureType textureType = texture->GetType();
				const TextureParameters textureParameters = texture->GetParameters();

				if (textureType == TextureType::Texture2D)
				{
					texture.reset();
					texture = Texture2D::CreateFromFile(name, filePath, textureParameters);
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

void TRAP::Graphics::TextureManager::Reload(const std::unique_ptr<Texture>& texture)
{
	for(auto& s_texture : s_textures)
	{
		if(s_texture == texture)
		{
			const std::string name = texture->GetName();
			const std::string filepath = texture->GetImage()->GetFilePath();
			if(filepath.empty())
			{
				TP_WARN("[TextureManager] Could not find Texture: \"", name, "\" to reload.");
				return;
			}

			const TextureType textureType = texture->GetType();
			const TextureParameters textureParameters = texture->GetParameters();

			if (textureType == TextureType::Texture2D)
			{
				s_texture.reset();
				s_texture = Texture2D::CreateFromFile(name, filepath, textureParameters);
				TP_INFO("[TextureManager] Reloaded: \"", name, "\"");
			}
			else
				TP_WARN("[TextureManager] Unknown TextureType: \"", name, "\"");

			return;
		}
	}

	TP_WARN("[TextureManager] Could not find Texture: \"", texture->GetName(), "\" to reload.");
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