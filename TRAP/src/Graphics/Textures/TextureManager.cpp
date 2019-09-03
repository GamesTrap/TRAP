#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture2D.h"
#include "TextureCube.h"

std::vector<std::unique_ptr<TRAP::Graphics::Texture>> TRAP::Graphics::TextureManager::s_textures;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(std::unique_ptr<Texture> texture)
{
	s_textures.push_back(std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Get(const std::string& name, const TextureType textureType)
{
	for (const auto& texture : s_textures)
		if (texture->GetName() == name && texture->GetType() == textureType)
			return texture.get();

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);
	if (textureType == TextureType::TextureCube)
		return Get("FallbackCube", TextureType::TextureCube);

	return nullptr;
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
					TP_INFO("[TextureManager][Texture2D] Reloaded: \"", nameOrVirtualPath, "\"");
				}
				else if (textureType == TextureType::TextureCube)
				{
					const InputFormat inputFormat = dynamic_cast<TextureCube*>(texture.get())->GetInputFormat();

					std::array<std::string, 6> filePaths{};
					for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetImages().size(); i++)
						filePaths[i] = dynamic_cast<TextureCube*>(texture.get())->GetImages()[i]->GetFilePath();

					texture.reset();
					if (inputFormat == InputFormat::Vertical_Cross || inputFormat == InputFormat::Horizontal_Cross)
						texture = TextureCube::CreateFromCross(name, filePath, inputFormat, textureParameters);
					else
						texture = TextureCube::CreateFromFiles(name, filePaths, textureParameters);

					TP_INFO("[TextureManager][TextureCube] Reloaded: \"", nameOrVirtualPath, "\"");
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
			if (texture->GetType() == TextureType::Texture2D)
			{
				if (nameOrVirtualPath == texture->GetImage()->GetFilePath())
				{
					Reload(texture);
					return;
				}
			}
			else if(texture->GetType() == TextureType::TextureCube)
			{
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetImages().size(); i++)
				{
					if (dynamic_cast<TextureCube*>(texture.get())->GetImages()[i])
					{
						if (nameOrVirtualPath == dynamic_cast<TextureCube*>(texture.get())->GetImages()[i]->GetFilePath())
						{
							Reload(texture);
							return;
						}
					}
				}
			}
		TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Reload(const std::unique_ptr<Texture>& texture)
{
	for (auto& s_texture : s_textures)
	{
		if (s_texture == texture)
		{
			const std::string name = texture->GetName();
			const std::string filepath = texture->GetImage()->GetFilePath();
			if (filepath.empty())
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
				TP_INFO("[TextureManager][Texture2D] Reloaded: \"", name, "\"");
			}
			else if (textureType == TextureType::TextureCube)
			{
				const InputFormat inputFormat = dynamic_cast<TextureCube*>(texture.get())->GetInputFormat();

				std::array<std::string, 6> filePaths{};
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetImages().size(); i++)
					filePaths[i] = dynamic_cast<TextureCube*>(texture.get())->GetImages()[i]->GetFilePath();

				s_texture.reset();
				if (inputFormat == InputFormat::Vertical_Cross || inputFormat == InputFormat::Horizontal_Cross)
					s_texture = TextureCube::CreateFromCross(name, filepath, inputFormat, textureParameters);
				else
					s_texture = TextureCube::CreateFromFiles(name, filePaths, textureParameters);

				TP_INFO("[TextureManager][TextureCube] Reloaded: \"", name, "\"");
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