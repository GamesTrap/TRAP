#include <filesystem>

#include "Version.h"
#include "Utils.h"
#include "Shader.h"

using namespace std::string_view_literals;

[[nodiscard]] bool CheckForParameters(const std::vector<std::string_view>& args, std::filesystem::path& outOutputPath,
                                      std::vector<std::array<std::string, 2>>& outCustomMacros);
void PrintUsage(const std::filesystem::path& programName);
void PrintVersion();

//-------------------------------------------------------------------------------------------------------------------//

int main(const int argc, const char* const* const argv)
{
	std::vector<std::string_view> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));

	std::filesystem::path outputPath{};
	std::vector<std::array<std::string, 2>> customMacros{};
	if(!CheckForParameters(args, outputPath, customMacros))
		return 0;

	//Print all customMacros
	for(const auto& macro : customMacros)
		std::cout << std::get<0>(macro) << " " << std::get<1>(macro) << std::endl;

	Shader shader{};
	if(!LoadShader(args[1], shader))
		return -1;

	if(!PreProcessGLSL(shader, customMacros))
		return -1;
	if(!ValidateShaderStages(shader))
		return -1;

	if(CompileGLSLToSPIRV(shader))
	{
		if(!SaveSPIRV(shader, outputPath))
			return -1;
	}
	else
		return -1;

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool CheckForParameters(const std::vector<std::string_view>& args, std::filesystem::path& outOutputPath,
                                      std::vector<std::array<std::string, 2>>& outCustomMacros)
{
	//Check for no parameters
	if (args.size() < 2)
	{
		PrintUsage(args[0]);
		return false;
	}

	//Check for help flag
	for(std::size_t i = 0; i < args.size(); ++i)
	{
		if(args[i] == "-h"sv || args[i] == "--help"sv)
		{
			PrintUsage(args[0]);
			return false;
		}
	}

	//Check for version flag
	for(const std::string_view arg : args)
	{
		if(arg == "--version"sv)
		{
			PrintVersion();
			return false;
		}
	}

	//Check for custom output file name
	for(std::size_t i = 0; i < args.size(); ++i)
	{
		if(args[i] == "-o"sv || args[i] == "--output"sv)
		{
			if(i + 1 < args.size())
			{
				outOutputPath = args[i + 1];
				break;
			}

			std::cerr << "No output file name specified!\n";
			return false;
		}
	}

	//Check for custom macros
	for(std::size_t i = 0; i < args.size(); ++i)
	{
		if(args[i] == "-m"sv || args[i] == "--macro"sv)
		{
			if(i + 1 < args.size())
			{
				std::string macro = std::string(args[i + 1]);
				std::size_t equalSign = macro.find('=');
				if(equalSign == std::string::npos)
				{
					std::cerr << "Invalid macro: \"" << macro << "\"\n";
					std::cerr << "Skipping macro\n";
					continue;
				}

				std::string name = macro.substr(0, equalSign);
				std::string value = macro.substr(equalSign + 1);
				outCustomMacros.push_back({ name, value });
			}
			else
			{
				std::cerr << "No macro specified!\n";
				return false;
			}
		}
	}

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