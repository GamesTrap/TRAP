#include "TRAPPCH.h"
#include "ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>> TRAP::Graphics::ShaderManager::s_Shaders{};

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadFile(const std::string& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(filepath);
	if(shader)
	{
		const std::string name = shader->GetName();
		
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Fallback");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadFile(const std::string& name, const std::string& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(name, filepath);

	if(shader)
	{
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Fallback");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::LoadSource(const std::string& name,
																			   const std::string& glslSource)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromSource(name, glslSource);

	if(shader)
	{
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Fallback");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(Scope<Shader> shader)
{
	TP_PROFILE_FUNCTION();

	if (shader)
	{
		if(!Exists(shader->GetName()))
			s_Shaders[shader->GetName()] = std::move(shader);			
		else
			TP_ERROR(Log::ShaderManagerPrefix, "Shader with Name: \"", shader->GetName(), "\" already exists! Ignoring new Shader");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Remove(const Scope<Shader>& shader)
{
	TP_PROFILE_FUNCTION();
	
	if(shader)
	{
		if (Exists(shader->GetName()))
			s_Shaders.erase(shader->GetName());
		else
			TP_ERROR(Log::ShaderManagerPrefix, "Could not find Shader with Name: \"", shader->GetName(), "\"!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();
	
	if (Exists(name))
		s_Shaders.erase(name);
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Could not find Shader with Name: \"", name, "\"!");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Get(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		return s_Shaders[name];

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find Shader: ", name, "!");
	TP_WARN(Log::ShaderManagerPrefix, "Using Fallback Shader!");
	
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

void TRAP::Graphics::ShaderManager::Reload(const std::string& nameOrVirtualPath)
{
	TP_PROFILE_FUNCTION();
	
	//Name
	if (nameOrVirtualPath[0] != '/')
	{
		if(Exists(nameOrVirtualPath))
		{
			const std::string path = s_Shaders[nameOrVirtualPath]->GetFilePath();
			std::string error;
			if (!path.empty())
			{
				s_Shaders[nameOrVirtualPath].reset();
				s_Shaders[nameOrVirtualPath] = Shader::CreateFromFile(nameOrVirtualPath, path);
				TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", nameOrVirtualPath, "\"");
			}
		}
		else
			TP_WARN(Log::ShaderManagerPrefix, "Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
	else //Virtual Path
	{		
		for (const auto& [name, shader] : s_Shaders)
			if (nameOrVirtualPath == shader->GetFilePath())
			{
				Reload(shader);
				return;
			}
		
		TP_WARN(Log::ShaderManagerPrefix, "Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Reload(const Scope<Shader>& shader)
{
	TP_PROFILE_FUNCTION();
	
	if(Exists(shader->GetName()))
	{
		const std::string name = shader->GetName();
		const std::string path = shader->GetFilePath();
		std::string error;
		if (!path.empty())
		{
			s_Shaders[name].reset();
			s_Shaders[name] = Shader::CreateFromFile(name, path);
			TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", name, "\"");
		}
	}
	else
		TP_WARN(Log::ShaderManagerPrefix, "Could not find Shader: \"", shader->GetName(), "\" to reload.");
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

	TP_DEBUG(Log::ShaderManagerPrefix, "Destroying Shaders");
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::Exists(const std::string& name)
{
	return s_Shaders.find(name) != s_Shaders.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::ExistsVirtualPath(const std::string_view virtualPath)
{
	for(const auto& [name, shader] : s_Shaders)
		if (shader->GetFilePath() == virtualPath)
			return true;

	return false;
}