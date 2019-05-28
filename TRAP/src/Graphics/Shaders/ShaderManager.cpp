#include "TRAPPCH.h"
#include "ShaderManager.h"

namespace TRAP::Graphics
{
	std::vector<std::unique_ptr<API::Shader>> ShaderManager::s_Shaders;

	void ShaderManager::Add(std::unique_ptr<API::Shader> shader)
	{
		if (shader)
			s_Shaders.push_back(std::move(shader));
	}

	API::Shader* ShaderManager::Get(const std::string_view name)
	{
		for (const auto& shader : s_Shaders)
			if (shader->GetName() == name)
				return shader.get();

		//Should always be available as a fallback
		return Get("Passthrough");
	}

	void ShaderManager::Clean()
	{
		s_Shaders.clear();
		TP_DEBUG("[ShaderManager] Cleared");
	}

	void ShaderManager::Reload(const std::string& nameOrVirtualPath)
	{
		//Name
		if (nameOrVirtualPath[0] != '/')
		{
			for (auto& s_Shader : s_Shaders)
			{
				if (s_Shader->GetName() == nameOrVirtualPath)
				{
					const std::string VSPath = s_Shader->GetVSFilePath();
					const std::string FSPath = s_Shader->GetFSFilePath();
					const std::string GSPath = s_Shader->GetGSFilePath();
					const std::string TCSPath = s_Shader->GetTCSFilePath();
					const std::string TESPath = s_Shader->GetTESFilePath();
					const std::string CSPath = s_Shader->GetCSFilePath();
					std::string error;
					if (VSPath.empty() && FSPath.empty() && GSPath.empty() && TCSPath.empty() && TESPath.empty() && CSPath.empty())
					{
						const std::string VSSource = s_Shader->GetVSSource();
						const std::string FSSource = s_Shader->GetFSSource();
						const std::string GSSource = s_Shader->GetGSSource();
						const std::string TCSSource = s_Shader->GetTCSSource();
						const std::string TESSource = s_Shader->GetTESSource();
						const std::string CSSource = s_Shader->GetCSSource();

						s_Shader.reset();
						s_Shader = API::Shader::CreateFromSource(nameOrVirtualPath, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
						TP_INFO("[ShaderManager] Reloaded: \"", nameOrVirtualPath, "\"");
					}
					else
					{
						s_Shader.reset();
						s_Shader = API::Shader::CreateFromFile(nameOrVirtualPath, VSPath, FSPath, GSPath, TCSPath, TESPath, CSPath,
						                                       s_Shader.get());
						TP_INFO("[ShaderManager] Reloaded: \"", nameOrVirtualPath, "\"");
					}

					return;
				}
			}
			TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
		}
		else
		{
			//Virtual Path
			for(const auto& shader : s_Shaders)
			{
				if(nameOrVirtualPath == shader->GetVSFilePath())
				{
					Reload(shader);
					return;
				}
				if (nameOrVirtualPath == shader->GetFSFilePath())
				{
					Reload(shader);
					return;
				}
				if (nameOrVirtualPath == shader->GetGSFilePath())
				{
					Reload(shader);
					return;
				}
				if (nameOrVirtualPath == shader->GetTCSFilePath())
				{
					Reload(shader);
					return;
				}
				if (nameOrVirtualPath == shader->GetTESFilePath())
				{
					Reload(shader);
					return;
				}
				if (nameOrVirtualPath == shader->GetCSFilePath())
				{
					Reload(shader);
					return;
				}
			}
			TP_WARN("[ShaderManager] Could not find Shader: \"", nameOrVirtualPath, "\" to reload.");
		}
	}

	void ShaderManager::Reload(const std::unique_ptr<API::Shader>& shader)
	{
		for (auto& s_Shader : s_Shaders)
		{
			if (s_Shader == shader)
			{
				const std::string name = shader->GetName();
				const std::string VSPath = shader->GetVSFilePath();
				const std::string FSPath = shader->GetFSFilePath();
				const std::string GSPath = shader->GetGSFilePath();
				const std::string TCSPath = shader->GetTCSFilePath();
				const std::string TESPath = shader->GetTESFilePath();
				const std::string CSPath = shader->GetCSFilePath();
				std::string error;
				if (VSPath.empty() && FSPath.empty() && GSPath.empty() && TCSPath.empty() && TESPath.empty() && CSPath.empty())
				{
					const std::string VSSource = shader->GetVSSource();
					const std::string FSSource = shader->GetFSSource();
					const std::string GSSource = shader->GetGSSource();
					const std::string TCSSource = shader->GetTCSSource();
					const std::string TESSource = shader->GetTESSource();
					const std::string CSSource = shader->GetCSSource();

					s_Shader.reset();
					s_Shader = API::Shader::CreateFromSource(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
					TP_INFO("[ShaderManager] Reloaded: \"", name, "\"");
				}
				else
				{
					s_Shader.reset();
					s_Shader = API::Shader::CreateFromFile(name, VSPath, FSPath, GSPath, TCSPath, TESPath, CSPath, shader.get());
					TP_INFO("[ShaderManager] Reloaded: \"", name, "\"");
				}

				return;
			}
		}
		TP_WARN("[ShaderManager] Could not find Shader: \"", shader->GetName(), "\" to reload.");
	}

	void ShaderManager::ReloadAll()
	{
		TP_INFO("[ShaderManager] Reloading all may take a while...");
		for (auto& shader : s_Shaders)
			Reload(shader);
	}

	void ShaderManager::Shutdown()
	{
		if (API::Shader::s_CurrentlyBound)
			API::Shader::s_CurrentlyBound->Unbind();

		TP_DEBUG("[ShaderManager] Destroying Shaders");
		for (auto& shader : s_Shaders)
			shader.reset();
	}
}