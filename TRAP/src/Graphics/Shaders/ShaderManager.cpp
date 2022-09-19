#include "TRAPPCH.h"
#include "ShaderManager.h"

#include "Embed.h"
#include "FileSystem/FileSystem.h"

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Shader>> Shaders{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadFile(const std::filesystem::path& filepath,
    																      const std::vector<Shader::Macro>* const userMacros)
{
	TP_PROFILE_FUNCTION();

	Ref<Shader> shader = Shader::CreateFromFile(filepath, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	const std::string name = shader->GetName();

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadFile(const std::string& name,
																          const std::filesystem::path& filepath,
																          const std::vector<Shader::Macro>* const userMacros)
{
	TP_PROFILE_FUNCTION();

	Ref<Shader> shader = Shader::CreateFromFile(name, filepath, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadSource(const std::string& name,
														              		const std::string& glslSource,
																      		const std::vector<Shader::Macro>* const userMacros)
{
	TP_PROFILE_FUNCTION();

	Ref<Shader> shader = Shader::CreateFromSource(name, glslSource, userMacros);

	if(!shader)
		return Get("FallbackGraphics");

	Add(std::move(shader));

	return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(Ref<Shader> shader)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(shader, "Provided shader is nullptr!");

	if(!Exists(shader->GetName()))
		Shaders[shader->GetName()] = std::move(shader);
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Shader with name: \"", shader->GetName(), "\" already exists! Ignoring new shader");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Remove(Ref<Shader> shader)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(shader, "Provided shader is nullptr!");

	if (Exists(shader->GetName()))
	{
		Ref<Shader> revShader = std::move(Shaders[shader->GetName()]);
		Shaders.erase(shader->GetName());
		return revShader;
	}
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", shader->GetName(), "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
	{
		Ref<Shader> shader = std::move(Shaders[name]);
		Shaders.erase(name);
		return shader;
	}
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Get(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		return Shaders[name];

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: ", name, "!");
	TP_WARN(Log::ShaderManagerPrefix, "Using fallback shader!");

	//Should always be available as a fallback
	return Get("FallbackGraphics");
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Shader>>& TRAP::Graphics::ShaderManager::GetShaders()
{
	return Shaders;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Clean()
{
	Shaders.clear();

	//Make sure that fallback shaders are always available
	Graphics::ShaderManager::LoadSource("FallbackGraphics", std::string(Embed::FallbackGraphicsShader));
	Graphics::ShaderManager::LoadSource("FallbackCompute", std::string(Embed::FallbackComputeShader));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Reload(const std::string& nameOrPath)
{
	TP_PROFILE_FUNCTION();

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			auto shader = Shaders[nameOrPath];
			if(shader->Reload())
				TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

			return shader;
		}
		else
			TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, shader] : Shaders)
		{
			if (FileSystem::IsPathEquivalent(nameOrPath, shader->GetFilePath()))
			{
				if(shader->Reload())
					TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

				return shader;
			}
		}

		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"",
		        std::filesystem::path(nameOrPath).u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Reload(Ref<Shader> shader)
{
	TP_PROFILE_FUNCTION();

	if(!Exists(shader->GetName()))
	{
		TP_WARN(Log::ShaderManagerPrefix, "Couldn't find shader: \"", shader->GetName(), "\" to reload.");
		return nullptr;
	}

	if(shader->Reload())
		TP_INFO(Log::ShaderManagerPrefix, "Reloaded: \"", shader->GetName(), "\"");

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::ReloadAll()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::ShaderManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, shader] : Shaders)
		Reload(shader);
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
		if (FileSystem::IsPathEquivalent(shader->GetFilePath(), path))
			return true;
	}

	return false;
}