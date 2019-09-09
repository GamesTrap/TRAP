#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture2D.h"
#include "TextureCube.h"

std::unordered_map<std::string, std::unique_ptr<TRAP::Graphics::Texture>> TRAP::Graphics::TextureManager::s_Textures;

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Load(const std::string& filepath, const TextureParameters parameters)
{
	std::unique_ptr<Texture> texture = Texture2D::CreateFromFile(filepath, parameters);
	const std::string name = texture->GetName();

	Add(std::move(texture));

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Load(const std::string& name, const std::string& filepath, const TextureParameters parameters)
{
	std::unique_ptr<Texture> texture = Texture2D::CreateFromFile(name, filepath, parameters);

	Add(std::move(texture));

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Load(const std::string& name, const std::string& filepath, const InputFormat format, const TextureParameters parameters)
{
	std::unique_ptr<Texture> texture = TextureCube::CreateFromCross(name, filepath, format, parameters);

	Add(std::move(texture));

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Load(const std::string& filepath, const InputFormat format, const TextureParameters parameters)
{
	std::unique_ptr<Texture> texture = TextureCube::CreateFromCross(filepath, format, parameters);
	const std::string name = texture->GetName();

	Add(std::move(texture));

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Load(const std::string& name, const std::array<std::string, 6> & filepaths, TextureParameters parameters)
{
	std::unique_ptr<Texture> texture = TextureCube::CreateFromFiles(name, filepaths, parameters);

	Add(std::move(texture));

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(std::unique_ptr<Texture> texture)
{
	if(texture)
	{
		if (!Exists(texture->GetName()))
			s_Textures[texture->GetName()] = std::move(texture);
		else
			TP_ERROR("[TextureManager] Texture with Name: ", texture->GetName(), " already exists!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Get(const std::string& name, const TextureType textureType)
{
	if(Exists(name))
		if (s_Textures[name]->GetType() == textureType)
			return s_Textures[name];

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);
	
	return Get("FallbackCube", TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::Texture2D>& TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	return reinterpret_cast<const std::unique_ptr<Texture2D>&>(Get(name, TextureType::Texture2D));
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unique_ptr<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	return reinterpret_cast<const std::unique_ptr<TextureCube>&>(Get(name, TextureType::TextureCube));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	s_Textures.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Reload(const std::string& nameOrVirtualPath)
{
	//Name
	if (nameOrVirtualPath[0] != '/')
	{
		if(Exists(nameOrVirtualPath))
		{
			const std::string filePath = s_Textures[nameOrVirtualPath]->GetImage()->GetFilePath();
			if (filePath.empty())
			{
				TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
				return;
			}

			const std::string name = s_Textures[nameOrVirtualPath]->GetName();
			const TextureType textureType = s_Textures[nameOrVirtualPath]->GetType();
			const TextureParameters textureParameters = s_Textures[nameOrVirtualPath]->GetParameters();

			if (textureType == TextureType::Texture2D)
			{
				s_Textures[nameOrVirtualPath].reset();
				s_Textures[nameOrVirtualPath] = Texture2D::CreateFromFile(name, filePath, textureParameters);
				TP_INFO("[TextureManager][Texture2D] Reloaded: \"", nameOrVirtualPath, "\"");
			}
			else if (textureType == TextureType::TextureCube)
			{
				const InputFormat inputFormat = dynamic_cast<TextureCube*>(s_Textures[nameOrVirtualPath].get())->GetInputFormat();

				std::array<std::string, 6> filePaths{};
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(s_Textures[nameOrVirtualPath].get())->GetImages().size(); i++)
					filePaths[i] = dynamic_cast<TextureCube*>(s_Textures[nameOrVirtualPath].get())->GetImages()[i]->GetFilePath();

				s_Textures[nameOrVirtualPath].reset();
				if (inputFormat == InputFormat::Vertical_Cross || inputFormat == InputFormat::Horizontal_Cross)
					s_Textures[nameOrVirtualPath] = TextureCube::CreateFromCross(name, filePath, inputFormat, textureParameters);
				else
					s_Textures[nameOrVirtualPath] = TextureCube::CreateFromFiles(name, filePaths, textureParameters);

				TP_INFO("[TextureManager][TextureCube] Reloaded: \"", nameOrVirtualPath, "\"");
			}
			else
				//Shouldn't be reached
				TP_WARN("[TextureManager] Unknown TextureType: \"", nameOrVirtualPath, "\"");
		}
		else
			TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
	}
	else //Virtual Path
	{
		for (const auto& texture : s_Textures)
			if (texture.second->GetType() == TextureType::Texture2D)
			{
				if (nameOrVirtualPath == texture.second->GetImage()->GetFilePath())
				{
					Reload(texture.second);
					return;
				}
			}
			else if(texture.second->GetType() == TextureType::TextureCube)
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.second.get())->GetImages().size(); i++)
					if (dynamic_cast<TextureCube*>(texture.second.get())->GetImages()[i])
						if (nameOrVirtualPath == dynamic_cast<TextureCube*>(texture.second.get())->GetImages()[i]->GetFilePath())
						{
							Reload(texture.second);
							return;
						}
		
		TP_WARN("[TextureManager] Could not find Texture: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Reload(const std::unique_ptr<Texture>& texture)
{
	if(Exists(texture->GetName()))
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
			s_Textures[name].reset();
			s_Textures[name] = Texture2D::CreateFromFile(name, filepath, textureParameters);
			TP_INFO("[TextureManager][Texture2D] Reloaded: \"", name, "\"");
		}
		else if (textureType == TextureType::TextureCube)
		{
			const InputFormat inputFormat = dynamic_cast<TextureCube*>(texture.get())->GetInputFormat();

			std::array<std::string, 6> filePaths{};
			for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetImages().size(); i++)
				filePaths[i] = dynamic_cast<TextureCube*>(texture.get())->GetImages()[i]->GetFilePath();

			s_Textures[name].reset();
			if (inputFormat == InputFormat::Vertical_Cross || inputFormat == InputFormat::Horizontal_Cross)
				s_Textures[name] = TextureCube::CreateFromCross(name, filepath, inputFormat, textureParameters);
			else
				s_Textures[name] = TextureCube::CreateFromFiles(name, filePaths, textureParameters);

			TP_INFO("[TextureManager][TextureCube] Reloaded: \"", name, "\"");
		}
		else
			TP_WARN("[TextureManager] Unknown TextureType: \"", name, "\"");
	}
	else
		TP_WARN("[TextureManager] Could not find Texture: \"", texture->GetName(), "\" to reload.");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::ReloadAll()
{
	TP_INFO("[TextureManager] Reloading all may take a while...");
	for (auto& texture : s_Textures)
		Reload(texture.second);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown()
{
	TP_DEBUG("[TextureManager] Destroying Textures");
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::Exists(const std::string& name)
{
	return s_Textures.find(name) != s_Textures.end();
}