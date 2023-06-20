#include <filesystem>

#include <fmt/format.h>

#include "Version.h"
#include "Utils.h"
#include "Shader.h"

using namespace std::string_view_literals;

[[nodiscard]] bool CheckForParameters(const std::vector<std::string_view>& args,
                                      std::optional<std::filesystem::path>& outOutputPath,
                                      std::vector<Macro>& outCustomMacros);
void PrintUsage(const std::filesystem::path& programName);
void PrintVersion();
void PrintInfo(const std::filesystem::path& filePath);

//-------------------------------------------------------------------------------------------------------------------//

int main(const int argc, const char* const* const argv)
{
	const std::vector<std::string_view> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));

	std::optional<std::filesystem::path> outputPath{};
	std::vector<Macro> customMacros{};
	if(!CheckForParameters(args, outputPath, customMacros))
		return 0;

	Shader shader{};
	if(!LoadShader(args[1], shader))
		return -1;

	if(!PreProcessGLSL(shader, customMacros))
		return -1;

	if(!ValidateShaderStages(shader))
		return -1;

	if(!CompileGLSLToSPIRV(shader))
		return -1;

	if(!SaveSPIRV(shader, outputPath))
		return -1;

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForHelpParameter(const std::vector<std::string_view>& args)
{
	if(std::any_of(args.begin(), args.end(), [](const auto arg){return arg == "-h"sv || arg == "--help"sv;}))
	{
		PrintUsage(args[0]);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForVersionParameter(const std::vector<std::string_view>& args)
{
	if(std::any_of(args.begin(), args.end(), [](const auto arg){return arg == "--version"sv;}))
	{
		PrintVersion();
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForInfoParameter(const std::vector<std::string_view>& args)
{
	if(std::any_of(args.begin(), args.end(), [](const auto arg){return arg == "--info"sv;}) ||
	   std::filesystem::path(args[1]).extension() == (std::string(".") + std::string(ShaderFileEnding)))
	{
		PrintInfo(args[1]);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForOutputFilePath(const std::vector<std::string_view>& args,
                                          std::optional<std::filesystem::path>& outOutputPath)
{
	outOutputPath = std::nullopt;

	auto it = std::find_if(args.begin(), args.end(),
	                       [](const auto str){return str == "-o"sv || str == "--output"sv;});

	if(it == args.end())
		return false;

	++it;

	if(it == args.end())
	{
		std::cerr << "No output file name specified!\n";
		return true;
	}

	outOutputPath = *it;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<Macro> SplitMacro(const std::string_view macro)
{
	const std::size_t equalSign = macro.find('=');
	if(equalSign == std::string::npos)
	{
		std::cerr << "Invalid macro: \"" << macro << "\"\n";
		std::cerr << "Skipping macro\n";
		return std::nullopt;
	}

	const std::string_view name = macro.substr(0, equalSign);
	const std::string_view value = macro.substr(equalSign + 1);

	return Macro{name, value};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForCustomMacros(const std::vector<std::string_view>& args,
                                        std::vector<Macro>& outCustomMacros)
{
	outCustomMacros.clear();

	for(auto it = args.begin(); it != args.end(); ++it)
	{
		if(*it != "-m"sv && *it != "--macro"sv)
			continue;

		++it;

		if(it == args.end())
		{
			std::cerr << "No macro specified!\n";
			return true;
		}

		const auto macro = SplitMacro(*it);
		if(!macro)
			continue;

		outCustomMacros.push_back(*macro);
	}

	return !outCustomMacros.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForParameters(const std::vector<std::string_view>& args,
                                      std::optional<std::filesystem::path>& outOutputPath,
                                      std::vector<Macro>& outCustomMacros)
{
	//Check for no parameters
	if (args.size() < 2)
	{
		PrintUsage(args[0]);
		return false;
	}

	if(CheckForHelpParameter(args))
		return false;

	if(CheckForVersionParameter(args))
		return false;

	if(CheckForInfoParameter(args))
		return false;

	if(CheckForOutputFilePath(args, outOutputPath) && !outOutputPath)
		return false;

	if(CheckForCustomMacros(args, outCustomMacros) && outCustomMacros.empty())
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintUsage(const std::filesystem::path& programName)
{
	fmt::println("{} <file> [options]\n", programName.filename().string());
	fmt::println("Options:");
	fmt::println("-h | --help                    | Print this help");
	fmt::println("   | --version                 | Print the version number");
	fmt::println("   | --info                    | Retrieve information from an TP-SPV file");
	fmt::println("-o | --output <file>           | Set a custom output file name");
	fmt::println("-m | --macro \"<key>\"=\"<value>\" | Set custom macro(s)");
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintVersion()
{
	fmt::println("ConvertToSPIRV {}.{}.{}", CONVERTTOSPIRV_VERSION_MAJOR(CONVERTTOSPIRV_VERSION),
	                                        CONVERTTOSPIRV_VERSION_MINOR(CONVERTTOSPIRV_VERSION),
				                            CONVERTTOSPIRV_VERSION_PATCH(CONVERTTOSPIRV_VERSION));
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintTPSPVShaderInfo(const std::filesystem::path& filePath, const Shader& shader, const uint32_t versionNumber)
{
	fmt::println("File: {}", filePath.string());
	fmt::println("Format: TRAP-SPIRV v{}", versionNumber);
	fmt::println("Number of contained Shaders: {}\n", shader.SubShaderSources.size());

	for(std::size_t i = 0; i < shader.SubShaderSources.size(); ++i)
	{
		fmt::println("{}. Shader:", (i + 1));
		fmt::println("    Stage: {}", ShaderStageToString(shader.SubShaderSources[i].Stage));
		fmt::println("    Size in bytes: {}", shader.SubShaderSources[i].SPIRV.size() * sizeof(decltype(shader.SubShaderSources[i].SPIRV)::value_type));

		if(i < (shader.SubShaderSources.size() - 1))
			fmt::println("");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintInfo(const std::filesystem::path& filePath)
{
	const auto hasExtension = FileHasExtension(filePath, std::string("." + std::string(ShaderFileEnding)));
	if(!hasExtension)
		return;
	if(!hasExtension.value())
	{
		std::cerr << "Unsupported file extension for file \"" << filePath << "\"!" << std::endl;
		return;
	}

	const auto fileData = ReadFile(filePath);
	if(!fileData)
		return;

	Shader shader;
	uint32_t shaderVersion = 0;
	const bool res = ParseTPSPVShader(*fileData, shader, shaderVersion);
	if(!res)
	{
		std::cerr << "Invalid or corrupted shader!" << std::endl;
		return;
	}

	PrintTPSPVShaderInfo(filePath, shader, shaderVersion);
}
