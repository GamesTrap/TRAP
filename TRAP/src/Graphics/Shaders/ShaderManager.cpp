#include "TRAPPCH.h"
#include "ShaderManager.h"

#include "FS/FS.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>> Shaders{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::LoadFile(const std::filesystem::path& filepath,
																const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(filepath, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	const std::string name = shader->GetName();

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::LoadFile(const std::string& name,
																const std::filesystem::path& filepath,
																const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromFile(name, filepath, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::LoadSource(const std::string& name,
														          const std::string& glslSource,
																  const std::vector<Shader::Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	Scope<Shader> shader = Shader::CreateFromSource(name, glslSource, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(Scope<Shader> shader)
{
	TRAP_ASSERT(shader, "Provided shader is nullptr!");
	TP_PROFILE_FUNCTION();

	if(!Exists(shader->GetName()))
		Shaders[shader->GetName()] = std::move(shader);
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Shader with name: \"", shader->GetName(), "\" already exists! Ignoring new shader");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader>  TRAP::Graphics::ShaderManager::Remove(const Shader* const shader)
{
	TRAP_ASSERT(shader, "Provided shader is nullptr!");
	TP_PROFILE_FUNCTION();

	if (Exists(shader->GetName()))
	{
		Scope<Shader> revShader = std::move(Shaders[shader->GetName()]);
		Shaders.erase(shader->GetName());
		return revShader;
	}
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", shader->GetName(), "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
	{
		Scope<Shader> shader = std::move(Shaders[name]);
		Shaders.erase(name);
		return shader;
	}
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::Get(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		return Shaders[name].get();

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: ", name, "!");
	TP_WARN(Log::ShaderManagerPrefix, "Using fallback shader!");

	//Should always be available as a fallback
	return Get("FallbackGraphics");
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Shader>>& TRAP::Graphics::ShaderManager::GetShaders()
{
	return Shaders;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Clean()
{
	Shaders.clear();
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
			const std::filesystem::path path = Shaders[nameOrPath]->GetFilePath();
			std::string error;
			if (!path.empty())
			{
				const RendererAPI::ShaderStage stages = Shaders[nameOrPath]->GetShaderStages();
				Shaders[nameOrPath].reset();
				Shaders[nameOrPath] = Shader::CreateFromFile(nameOrPath, path, stages);
				TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", nameOrPath, "\"");
				return Shaders[nameOrPath].get();
			}
		}
		else
			TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, shader] : Shaders)
		{
			if (FS::IsPathEquivalent(nameOrPath, shader->GetFilePath()))
				return Reload(shader.get());
		}

		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"",
		        std::filesystem::path(nameOrPath).generic_u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader* TRAP::Graphics::ShaderManager::Reload(const Shader* const shader)
{
	TP_PROFILE_FUNCTION();

	if(!Exists(shader->GetName()))
	{
		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", shader->GetName(), "\" to reload.");
		return nullptr;
	}

	const std::string name = shader->GetName();
	const std::filesystem::path path = shader->GetFilePath();
	const RendererAPI::ShaderStage stages = Shaders[name]->GetShaderStages();

	if (path.empty())
		return nullptr;

	Shaders[name].reset();
	Shaders[name] = Shader::CreateFromFile(name, path, stages);
	TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", name, "\"");

	return Shaders[name].get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::ReloadAll()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::ShaderManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, shader] : Shaders)
		Reload(shader.get());
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
	return Shaders.find(name) != Shaders.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::ShaderManager::ExistsPath(const std::filesystem::path& path)
{
	for(const auto& [name, shader] : Shaders)
	{
		if (FS::IsPathEquivalent(shader->GetFilePath(), path))
			return true;
	}

	return false;
}