#include <filesystem>

#include "Version.h"
#include "Utils.h"
#include "Shader.h"

using namespace std::string_view_literals;

[[nodiscard]] bool CheckForParameters(const std::vector<std::string_view>& args,
                                      std::optional<std::filesystem::path>& outOutputPath,
                                      std::vector<Macro>& outCustomMacros);
void PrintUsage(const std::filesystem::path& programName);
void PrintVersion();

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

	if(CheckForOutputFilePath(args, outOutputPath) && !outOutputPath)
		return false;

	if(CheckForCustomMacros(args, outCustomMacros) && outCustomMacros.empty())
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintUsage(const std::filesystem::path& programName)
{
	std::cout << programName.filename().u8string() << " <file> [options]" << "\n\n" <<
	             "Options:\n" <<
				 "-h | --help                    | Print this help\n" <<
				 "   | --version                 | Print the version number\n" <<
				 "-o | --output <file>           | Set a custom output file name\n" <<
				 "-m | --macro \"<key>\"=\"<value>\" | Set custom macro(s)" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void PrintVersion()
{
	std::cout << "ConvertToSPIRV " << CONVERTTOSPIRV_VERSION_MAJOR(CONVERTTOSPIRV_VERSION) << '.'
	                               << CONVERTTOSPIRV_VERSION_MINOR(CONVERTTOSPIRV_VERSION) << '.'
			                       << CONVERTTOSPIRV_VERSION_PATCH(CONVERTTOSPIRV_VERSION) << std::endl;
}