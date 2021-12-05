#include "TRAPPCH.h"
#include "ShaderManager.h"

#include "FS/FS.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>> TRAP::Graphics::ShaderManager::s_Shaders{};

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadFile(const std::filesystem::path& filepath,
																				   const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(filepath, userMacros);

	if(!shader)
		return Get("Fallback");

	const std::string name = shader->GetName();

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadFile(const std::string& name,
																				   const std::filesystem::path& filepath,
																				   const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(name, filepath, userMacros);

	if(!shader)
		return Get("Fallback");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadSource(const std::string& name,
																			         const std::string& glslSource,
																			         const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromSource(name, glslSource, userMacros);

	if(!shader)
		return Get("Fallback");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(Scope<Shader> shader)
{
	TRAP_ASSERT(shader, "Provided shader is nullptr!");
	TP_PROFILE_FUNCTION();

	if(!Exists(shader->GetName()))
		s_Shaders[shader->GetName()] = std::move(shader);
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Shader with name: \"", shader->GetName(), "\" already exists! Ignoring new shader");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Remove(const Scope<Shader>& shader)
{
	TRAP_ASSERT(shader, "Provided shader is nullptr!");
	TP_PROFILE_FUNCTION();

	if (Exists(shader->GetName()))
		s_Shaders.erase(shader->GetName());
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", shader->GetName(), "\"!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
		s_Shaders.erase(name);
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", name, "\"!");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Get(const std::string& name) //TODO Return a pointer instead
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		return s_Shaders[name];

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: ", name, "!");
	TP_WARN(Log::ShaderManagerPrefix, "Using fallback shader!");

	//Should always be available as a fallback
	return Get("Fallback");
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>>& TRAP::Graphics::ShaderManager::GetShaders()
{
	return s_Shaders;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Clean()
{
	s_Shaders.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::Reload(const std::string& nameOrPath)
{
	TP_PROFILE_FUNCTION();

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			const std::filesystem::path path = s_Shaders[nameOrPath]->GetFilePath();
			std::string error;
			if (!path.empty())
			{
				s_Shaders[nameOrPath].reset();
				s_Shaders[nameOrPath] = Shader::CreateFromFile(nameOrPath, path);
				TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", nameOrPath, "\"");
				return s_Shaders[nameOrPath].get();
			}
		}
		else
			TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, shader] : s_Shaders)
		{
			if (FS::IsPathEquivalent(nameOrPath, shader->GetFilePath()))
				return Reload(shader);
		}

		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"",
		        std::filesystem::path(nameOrPath).generic_u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::Reload(const Scope<Shader>& shader)
{
	TP_PROFILE_FUNCTION();

	if(!Exists(shader->GetName()))
	{
		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", shader->GetName(), "\" to reload.");
		return nullptr;
	}

	const std::string name = shader->GetName();
	const std::filesystem::path path = shader->GetFilePath();
	std::string error;

	if (path.empty())
		return nullptr;

	s_Shaders[name].reset();
	s_Shaders[name] = Shader::CreateFromFile(name, path);
	TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", name, "\"");

	return s_Shaders[name].get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::ReloadAll()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::ShaderManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, shader] : s_Shaders)
		Reload(shader);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Shutdown()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::ShaderManagerPrefix, "Destroying shaders");
#endif
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::Exists(const std::string& name)
{
	return s_Shaders.find(name) != s_Shaders.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::ExistsPath(const std::filesystem::path& path)
{
	for(const auto& [name, shader] : s_Shaders)
	{
		if (FS::IsPathEquivalent(shader->GetFilePath(), path))
			return true;
	}

	return false;
}