#include "Utils.h"
#include "Shader.h"
#include <filesystem>

using namespace std::string_view_literals;

bool CheckForParameters(int argc, char* argv[], std::filesystem::path& outOutputPath,
                        std::vector<std::array<std::string, 2>>& outCustomMacros);
void PrintUsage(const std::filesystem::path& programName);

//-------------------------------------------------------------------------------------------------------------------//

int main(const int argc, char* argv[])
{
	std::filesystem::path outputPath{};
	std::vector<std::array<std::string, 2>> customMacros{};
	if(!CheckForParameters(argc, argv, outputPath, customMacros))
		return 0;

	//Print all customMacros
	for(const auto& macro : customMacros)
		std::cout << macro[0] << " " << macro[1] << std::endl;

	Shader shader{};
	if(!LoadShader(argv[1], shader))
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

bool CheckForParameters(const int argc, char* argv[], std::filesystem::path& outOutputPath,
                        std::vector<std::array<std::string, 2>>& outCustomMacros)
{
	//Check for no parameters
	if (argc < 2)
	{
		PrintUsage(argv[0]);
		return false;
	}

	//Check for help flag
	for(int32_t i = 0; i < argc; ++i)
	{
		if(argv[i] == "-h"sv || argv[i] == "--help"sv)
		{
			PrintUsage(argv[0]);
			return false;
		}
	}

	//Check for custom output file name
	for(int32_t i = 0; i < argc; ++i)
	{
		if(argv[i] == "-o"sv || argv[i] == "--output"sv)
		{
			if(i + 1 < argc)
			{
				outOutputPath = argv[i + 1];
				break;
			}
			else
			{
				std::cerr << "No output file name specified!\n";
				return false;
			}
		}
	}

	//Check for custom macros
	for(int32_t i = 0; i < argc; ++i)
	{
		if(argv[i] == "-m"sv || argv[i] == "--macro"sv)
		{
			if(i + 1 < argc)
			{
				std::string macro = argv[i + 1];
				std::size_t equalSign = macro.find('=');
				if(equalSign == std::string::npos)
				{
					std::cerr << "Invalid macro: \"" << macro << "\"\n";
					std::cerr << "Skipping macro\n";
					continue;
				}
				else
				{
					std::string name = macro.substr(0, equalSign);
					std::string value = macro.substr(equalSign + 1);
					outCustomMacros.push_back({ name, value });
				}
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
	std::cout << programName.filename().u8string() << " <file> [options]" << '\n' <<
	             "Options:\n" <<
				 "-h | --help          Print this help\n" <<
				 "-o | --output <file> Set a custom output file name\n" <<
				 "-m | --macro \"<key>\"=\"<value>\" Set custom macro(s)\n";
}