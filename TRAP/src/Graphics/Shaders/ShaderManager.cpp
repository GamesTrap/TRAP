#include "TRAPPCH.h"
#include "ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>> TRAP::Graphics::ShaderManager::s_Shaders;

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Load(const std::string& filepath)
{
	Scope<Shader> shader = Shader::CreateFromFile(filepath);
	if(shader)
	{
		const std::string name = shader->GetName();
		
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Passthrough");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Load(const std::string& name, const std::string& filepath)
{
	Scope<Shader> shader = Shader::CreateFromFile(name, filepath);

	if(shader)
	{
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Passthrough");
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Load(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSSource)
{
	Scope<Shader> shader = Shader::CreateFromSource(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSSource);

	if(shader)
	{
		Add(std::move(shader));
		
		return Get(name);
	}

	return Get("Passthrough");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(Scope<Shader> shader)
{
	if (shader)
	{
		if(!Exists(shader->GetName()))
			s_Shaders[shader->GetName()] = std::move(shader);			
		else
			TP_ERROR("[ShaderManager] Shader with Name: ", shader->GetName(), " already exists!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Shader>& TRAP::Graphics::ShaderManager::Get(const std::string& name)
{
	if(Exists(name))
			return s_Shaders[name];
	
	//Should always be available as a fallback
	return Get("Passthrough");
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
	//Name
	if (nameOrVirtualPath[0] != '/')
	{
		if(Exists(nameOrVirtualPath))
		{
			const std::string path = s_Shaders[nameOrVirtualPath]->GetFilePath();
			std::string error;
			if (!path.empty())
			{
				s_Shaders[nameOrVirtualPath]->Unbind();
				s_Shaders[nameOrVirtualPath].reset();
				s_Shaders[nameOrVirtualPath] = Shader::CreateFromFile(nameOrVirtualPath, path);
				TP_INFO("[ShaderManager] Reloaded: \"", nameOrVirtualPath, "\"");
			}
		}
		else
			TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
	else //Virtual Path
	{
		for (const auto& [name, shader] : s_Shaders)
			if (nameOrVirtualPath == shader->GetFilePath())
			{
				Reload(shader);
				return;
			}
		
		TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Reload(const Scope<Shader>& shader)
{
	if(Exists(shader->GetName()))
	{
		const std::string name = shader->GetName();
		const std::string path = shader->GetFilePath();
		std::string error;
		if (!path.empty())
		{
			s_Shaders[name]->Unbind();
			s_Shaders[name].reset();
			s_Shaders[name] = Shader::CreateFromFile(name, path);
			TP_INFO("[ShaderManager] Reloaded: \"", name, "\"");
		}
	}
	else
		TP_WARN("[ShaderManager] Could not find Shader: \"", shader->GetName(), "\" to reload.");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::ReloadAll()
{
	TP_INFO("[ShaderManager] Reloading all may take a while...");
	for (auto& [name, shader] : s_Shaders)
		Reload(shader);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Shutdown()
{
	if (Shader::s_CurrentlyBound)
		Shader::s_CurrentlyBound->Unbind();

	TP_DEBUG("[ShaderManager] Destroying Shaders");
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::Exists(const std::string& name)
{
	return s_Shaders.find(name) != s_Shaders.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::ExistsVirtualPath(const std::string& virtualPath)
{
	for(const auto& [name, shader] : s_Shaders)
		if (shader->GetFilePath() == virtualPath)
			return true;

	return false;
}