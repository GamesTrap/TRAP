#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "FS/FS.h"

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Texture>> Textures;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D* TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture2D> texture = Texture2D::CreateFromFile(filepath);
	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                            const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture2D::CreateFromFile(name, filepath);

	if(texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                            const Image* const img)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture2D::CreateFromImage(name, img);

	if (texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                              const std::filesystem::path& filepath,
																  const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<TextureCube> texture = TextureCube::CreateFromFile(name, filepath, format);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
	                                                              const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<TextureCube> texture = TextureCube::CreateFromFile(filepath, format);

	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                              const std::array<std::filesystem::path, 6>& filepaths)
{
	TP_PROFILE_FUNCTION();

	Scope<TextureCube> texture = TextureCube::CreateFromFiles(name, filepaths);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                              const Image* const img,
																  const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<TextureCube> texture = TextureCube::CreateFromImage(name, img, format);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(Scope<Texture> texture)
{
	TP_PROFILE_FUNCTION();

	if(texture)
	{
		if (!Exists(texture->GetName()))
			Textures[texture->GetName()] = std::move(texture);
		else
			TP_ERROR(Log::TextureManagerPrefix, "Texture with name: \"", texture->GetName(),
			         "\" already exists! Ignoring new texture");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const Texture* const texture)
{
	TP_PROFILE_FUNCTION();

	if(texture)
	{
		if (Exists(texture->GetName()))
		{
			Scope<Texture> tex = std::move(Textures[texture->GetName()]);
			Textures.erase(texture->GetName());
			return tex;
		}
		else
			TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
	{
		Scope<Texture> tex = std::move(Textures[name]);
		Textures.erase(name);
		return tex;
	}
	else
		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Get(const std::string& name,
                                                             const TextureType textureType)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		if (Textures[name]->GetType() == textureType)
				return Textures[name].get();

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);

	return Get("FallbackCube", TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D* TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	return dynamic_cast<Texture2D*>(Get(name, TextureType::Texture2D));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	return dynamic_cast<TextureCube*>(Get(name, TextureType::TextureCube));
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Texture>>& TRAP::Graphics::TextureManager::GetTextures()
{
	return Textures;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	Textures.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Reload(const std::string& nameOrPath)
{
	TP_PROFILE_FUNCTION();

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			const TextureType textureType = Textures[nameOrPath]->GetType();
			std::filesystem::path filePath;
			if (textureType == TextureType::Texture2D)
				filePath = dynamic_cast<Texture2D*>(Textures[nameOrPath].get())->GetFilePath();
			else
				filePath = dynamic_cast<TextureCube*>(Textures[nameOrPath].get())->GetFilePaths()[0];

			if (filePath.empty())
			{
				TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
				return nullptr;
			}

			const std::string name = Textures[nameOrPath]->GetName();

			if (textureType == TextureType::Texture2D)
			{
				Textures[nameOrPath].reset();
				Textures[nameOrPath] = Texture2D::CreateFromFile(name, filePath);
				Textures[nameOrPath]->AwaitLoading();
				TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", nameOrPath, "\"");
				return Textures[nameOrPath].get();
			}
			if (textureType == TextureType::TextureCube)
			{
				if (Textures[nameOrPath])
				{
					const TextureCubeFormat textureFormat = dynamic_cast<TextureCube*>
						(
							Textures[nameOrPath].get()
						)->GetTextureCubeFormat();

					std::array<std::filesystem::path, 6> filePaths{};
					for (uint32_t i = 0;
					     i < dynamic_cast<TextureCube*>(Textures[nameOrPath].get())->GetFilePaths().size();
						 i++)
						filePaths[i] = dynamic_cast<TextureCube*>(Textures[nameOrPath].get())->GetFilePaths()[i];

					Textures[nameOrPath].reset();
					if (textureFormat == TextureCubeFormat::Cross /*||
					    textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
						Textures[nameOrPath] = TextureCube::CreateFromFile(name, filePath, textureFormat);
					else
						Textures[nameOrPath] = TextureCube::CreateFromFiles(name, filePaths);
					Textures[nameOrPath]->AwaitLoading();

					TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", nameOrPath, "\"");
					return Textures[nameOrPath].get();
				}
			}
			else
				//Shouldn't be reached
				TP_WARN(Log::TextureManagerPrefix, "Unknown texture type: \"", nameOrPath, "\"");
		}
		else
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
	}
	else
	{
		//Path
		for (const auto& [name, texture] : Textures)
			if (texture->GetType() == TextureType::Texture2D)
			{
				if (FS::IsPathEquivalent(nameOrPath, dynamic_cast<Texture2D*>(texture.get())->GetFilePath()))
					return Reload(texture.get());
			}
			else if (texture->GetType() == TextureType::TextureCube)
			{
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetFilePaths().size(); i++)
				{
					if (!dynamic_cast<TextureCube*>(texture.get())->GetFilePaths()[i].empty())
					{
						if (FS::IsPathEquivalent(nameOrPath, dynamic_cast<TextureCube*>(texture.get())->GetFilePaths()[i]))
							return Reload(texture.get());
					}
				}
			}

		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"",
		        std::filesystem::path(nameOrPath).generic_u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Reload(const Texture* const texture)
{
	TP_PROFILE_FUNCTION();

	if(Exists(texture->GetName()))
	{
		const std::string name = texture->GetName();
		const TextureType textureType = Textures[name]->GetType();
		std::filesystem::path filePath;
		if (textureType == TextureType::Texture2D)
			filePath = dynamic_cast<Texture2D*>(Textures[name].get())->GetFilePath();
		else
			filePath = dynamic_cast<TextureCube*>(Textures[name].get())->GetFilePaths()[0];
		if (filePath.empty())
		{
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", name, "\" to reload.");
			return nullptr;
		}

		if (textureType == TextureType::Texture2D)
		{
			Textures[name].reset();
			Textures[name] = Texture2D::CreateFromFile(name, filePath);
			Textures[name]->AwaitLoading();
			TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", name, "\"");
			return Textures[name].get();
		}
		if (textureType == TextureType::TextureCube)
		{
			const TextureCubeFormat textureFormat = dynamic_cast<const TextureCube* const>
				(
					texture
				)->GetTextureCubeFormat();

			const TextureCube* const texCube = dynamic_cast<const TextureCube* const>(texture);
			std::array<std::filesystem::path, 6> filePaths{};
			for (uint32_t i = 0; i < texCube->GetFilePaths().size(); i++)
				filePaths[i] = texCube->GetFilePaths()[i];

			Textures[name].reset();
			if (textureFormat == TextureCubeFormat::Cross /*||
				textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
				Textures[name] = TextureCube::CreateFromFile(name, filePath, textureFormat);
			else
				Textures[name] = TextureCube::CreateFromFiles(name, filePaths);
			Textures[name]->AwaitLoading();

			TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", name, "\"");
			return Textures[name].get();
		}

		TP_WARN(Log::TextureManagerPrefix, "Unknown texture type: \"", name, "\"");
	}
	else
		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", texture->GetName(), "\" to reload.");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::ReloadAll()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::TextureManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, texture] : Textures)
		Reload(texture.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::TextureManagerPrefix, "Destroying textures");
#endif
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::Exists(const std::string& name)
{
	return Textures.find(name) != Textures.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::ExistsPath(const std::filesystem::path& path)
{
	for (const auto& [name, texture] : Textures)
	{
		if (texture->GetType() == TextureType::Texture2D)
		{
			if (FS::IsPathEquivalent(dynamic_cast<Texture2D*>(texture.get())->GetFilePath(), path))
				return true;
		}
		else if(texture->GetType() == TextureType::TextureCube)
		{
			const auto* textureCube = dynamic_cast<const TextureCube*>(texture.get());
			const std::array<std::filesystem::path, 6> imageFilePaths = textureCube->GetFilePaths();
			for(const auto& filePath : imageFilePaths)
			{
				if (FS::IsPathEquivalent(filePath, path))
					return true;
			}
		}
	}

	return false;
}