#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "FS/FS.h"

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Texture>> TRAP::Graphics::TextureManager::s_Textures;

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Texture2D>& TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath)
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

const TRAP::Scope<TRAP::Graphics::Texture2D>& TRAP::Graphics::TextureManager::Load(const std::string& name,
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

const TRAP::Scope<TRAP::Graphics::Texture2D>& TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                               const Scope<Image>& img)
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

const TRAP::Scope<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::Load(const std::string& name,
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

const TRAP::Scope<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
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

const TRAP::Scope<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::Load(const std::string& name,
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

const std::unique_ptr<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                                     const Scope<Image>& img,
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
			s_Textures[texture->GetName()] = std::move(texture);
		else
			TP_ERROR(Log::TextureManagerPrefix, "Texture with name: \"", texture->GetName(),
			         "\" already exists! Ignoring new texture");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Remove(const Scope<Texture>& texture)
{
	TP_PROFILE_FUNCTION();

	if(texture)
	{
		if (Exists(texture->GetName()))
			s_Textures.erase(texture->GetName());
		else
			TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
		s_Textures.erase(name);
	else
		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Texture>& TRAP::Graphics::TextureManager::Get(const std::string& name,
                                                                                const TextureType textureType)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		if (s_Textures[name]->GetType() == textureType)
				return s_Textures[name];

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);

	return Get("FallbackCube", TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Texture2D>& TRAP::Graphics::TextureManager::Get2D(const std::string& name) //TODO Return a pointer instead
{
	TP_PROFILE_FUNCTION();

	return reinterpret_cast<const Scope<Texture2D>&>(Get(name, TextureType::Texture2D));
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::TextureCube>& TRAP::Graphics::TextureManager::GetCube(const std::string& name) //TODO Return a pointer instead
{
	TP_PROFILE_FUNCTION();

	return reinterpret_cast<const Scope<TextureCube>&>(Get(name, TextureType::TextureCube));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	s_Textures.clear();
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
			const TextureType textureType = s_Textures[nameOrPath]->GetType();
			std::filesystem::path filePath;
			if (textureType == TextureType::Texture2D)
				filePath = dynamic_cast<Texture2D*>(s_Textures[nameOrPath].get())->GetFilePath();
			else
				filePath = dynamic_cast<TextureCube*>(s_Textures[nameOrPath].get())->GetFilePaths()[0];

			if (filePath.empty())
			{
				TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
				return nullptr;
			}

			const std::string name = s_Textures[nameOrPath]->GetName();

			if (textureType == TextureType::Texture2D)
			{
				s_Textures[nameOrPath].reset();
				s_Textures[nameOrPath] = Texture2D::CreateFromFile(name, filePath);
				s_Textures[nameOrPath]->AwaitLoading();
				TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", nameOrPath, "\"");
				return s_Textures[nameOrPath].get();
			}
			if (textureType == TextureType::TextureCube)
			{
				if (s_Textures[nameOrPath])
				{
					const TextureCubeFormat textureFormat = dynamic_cast<TextureCube*>
						(
							s_Textures[nameOrPath].get()
						)->GetTextureCubeFormat();

					std::array<std::filesystem::path, 6> filePaths{};
					for (uint32_t i = 0;
					     i < dynamic_cast<TextureCube*>(s_Textures[nameOrPath].get())->GetFilePaths().size();
						 i++)
						filePaths[i] = dynamic_cast<TextureCube*>(s_Textures[nameOrPath].get())->GetFilePaths()[i];

					s_Textures[nameOrPath].reset();
					if (textureFormat == TextureCubeFormat::Cross /*||
					    textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
						s_Textures[nameOrPath] = TextureCube::CreateFromFile(name, filePath, textureFormat);
					else
						s_Textures[nameOrPath] = TextureCube::CreateFromFiles(name, filePaths);
					s_Textures[nameOrPath]->AwaitLoading();

					TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", nameOrPath, "\"");
					return s_Textures[nameOrPath].get();
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
		for (const auto& [name, texture] : s_Textures)
			if (texture->GetType() == TextureType::Texture2D)
			{
				if (FS::IsPathEquivalent(nameOrPath, dynamic_cast<Texture2D*>(texture.get())->GetFilePath()))
					return Reload(texture);
			}
			else if (texture->GetType() == TextureType::TextureCube)
			{
				for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetFilePaths().size(); i++)
				{
					if (!dynamic_cast<TextureCube*>(texture.get())->GetFilePaths()[i].empty())
					{
						if (FS::IsPathEquivalent(nameOrPath, dynamic_cast<TextureCube*>(texture.get())->GetFilePaths()[i]))
							return Reload(texture);
					}
				}
			}

		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"",
		        std::filesystem::path(nameOrPath).generic_u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Reload(const Scope<Texture>& texture)
{
	TP_PROFILE_FUNCTION();

	if(Exists(texture->GetName()))
	{
		const std::string name = texture->GetName();
		const TextureType textureType = s_Textures[name]->GetType();
		std::filesystem::path filePath;
		if (textureType == TextureType::Texture2D)
			filePath = dynamic_cast<Texture2D*>(s_Textures[name].get())->GetFilePath();
		else
			filePath = dynamic_cast<TextureCube*>(s_Textures[name].get())->GetFilePaths()[0];
		if (filePath.empty())
		{
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", name, "\" to reload.");
			return nullptr;
		}

		if (textureType == TextureType::Texture2D)
		{
			s_Textures[name].reset();
			s_Textures[name] = Texture2D::CreateFromFile(name, filePath);
			s_Textures[name]->AwaitLoading();
			TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", name, "\"");
			return s_Textures[name].get();
		}
		if (textureType == TextureType::TextureCube)
		{
			const TextureCubeFormat textureFormat = dynamic_cast<TextureCube*>
				(
					texture.get()
				)->GetTextureCubeFormat();

			std::array<std::filesystem::path, 6> filePaths{};
			for (uint32_t i = 0; i < dynamic_cast<TextureCube*>(texture.get())->GetFilePaths().size(); i++)
				filePaths[i] = dynamic_cast<TextureCube*>(texture.get())->GetFilePaths()[i];

			s_Textures[name].reset();
			if (textureFormat == TextureCubeFormat::Cross /*||
				textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
				s_Textures[name] = TextureCube::CreateFromFile(name, filePath, textureFormat);
			else
				s_Textures[name] = TextureCube::CreateFromFiles(name, filePaths);
			s_Textures[name]->AwaitLoading();

			TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", name, "\"");
			return s_Textures[name].get();
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
	for (auto& [name, texture] : s_Textures)
		Reload(texture);
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
	return s_Textures.find(name) != s_Textures.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::ExistsPath(const std::filesystem::path& path)
{
	for (const auto& [name, texture] : s_Textures)
	{
		if (texture->GetType() == TextureType::Texture2D)
		{
			if (FS::IsPathEquivalent(dynamic_cast<Texture2D*>(texture.get())->GetFilePath(), path))
				return true;
		}
		else if(texture->GetType() == TextureType::TextureCube)
		{
			const std::array<std::filesystem::path, 6> imageFilePaths = reinterpret_cast<const Scope<TextureCube>&>(texture)->GetFilePaths();
			for(const auto& filePath : imageFilePaths)
			{
				if (FS::IsPathEquivalent(filePath, path))
					return true;
			}
		}
	}

	return false;
}