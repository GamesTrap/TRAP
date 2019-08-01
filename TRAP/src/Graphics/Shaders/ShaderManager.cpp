#include "TRAPPCH.h"
#include "ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Graphics::Shader>> TRAP::Graphics::ShaderManager::s_Shaders;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(std::unique_ptr<Shader> shader)
{
	if (shader)
		s_Shaders.push_back(std::move(shader));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::Get(const std::string_view name)
{
	for (const auto& shader : s_Shaders)
		if (shader->GetName() == name)
			return shader.get();

	//Should always be available as a fallback
	return Get("Passthrough");
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
		for (auto& shader : s_Shaders)
		{
			if (shader->GetName() == nameOrVirtualPath)
			{
				const std::string path = shader->GetFilePath();
				std::string error;
				if (path.empty())
				{
					const std::string VSSource = shader->GetVSSource();
					const std::string FSSource = shader->GetFSSource();
					const std::string GSSource = shader->GetGSSource();
					const std::string TCSSource = shader->GetTCSSource();
					const std::string TESSource = shader->GetTESSource();
					const std::string CSSource = shader->GetCSSource();

					shader.reset();
					shader = Shader::CreateFromSource(nameOrVirtualPath, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
					TP_INFO("[ShaderManager] Reloaded: \"", nameOrVirtualPath, "\"");
				}
				else
				{
					shader.reset();
					shader = Shader::CreateFromFile(nameOrVirtualPath, path, shader.get());
					TP_INFO("[ShaderManager] Reloaded: \"", nameOrVirtualPath, "\"");
				}

				return;
			}
		}
		TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
	else //Virtual Path
	{
		for (const auto& shader : s_Shaders)
		{
			if (nameOrVirtualPath == shader->GetFilePath())
			{
				Reload(shader);
				return;
			}
		}
		TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Reload(const std::unique_ptr<Shader>& shader)
{
	for (auto& s_Shader : s_Shaders)
	{
		if (s_Shader == shader)
		{
			const std::string name = shader->GetName();
			const std::string path = shader->GetFilePath();
			std::string error;
			if (path.empty())
			{
				const std::string VSSource = shader->GetVSSource();
				const std::string FSSource = shader->GetFSSource();
				const std::string GSSource = shader->GetGSSource();
				const std::string TCSSource = shader->GetTCSSource();
				const std::string TESSource = shader->GetTESSource();
				const std::string CSSource = shader->GetCSSource();

				s_Shader.reset();
				s_Shader = Shader::CreateFromSource(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
				TP_INFO("[ShaderManager] Reloaded: \"", name, "\"");
			}
			else
			{
				s_Shader.reset();
				s_Shader = Shader::CreateFromFile(name, path, shader.get());
				TP_INFO("[ShaderManager] Reloaded: \"", name, "\"");
			}

			return;
		}
	}
	TP_WARN("[ShaderManager] Could not find Shader: \"", shader->GetName(), "\" to reload.");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::ReloadAll()
{
	TP_INFO("[ShaderManager] Reloading all may take a while...");
	for (auto& shader : s_Shaders)
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