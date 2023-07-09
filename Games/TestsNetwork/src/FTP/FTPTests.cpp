#include "FTPTests.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

FTPTests::FTPTests()
	: Layer("FTPTests"), m_ftpThread(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void FTPTests::OnAttach()
{
	m_ftpThread = std::make_unique<std::thread>(FTP);
	if(m_ftpThread)
		m_ftpThread->detach();
}

//-------------------------------------------------------------------------------------------------------------------//

void FTPTests::OnDetach()
{
	m_ftpThread.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	constexpr std::string_view NetWorkFTPPrefix = "[Network][FTP] ";
}

void FTPTests::FTP()
{
	//Choose the server address
	TRAP::Network::IPv4Address address{};
	do
	{
		fmt::print("{}Enter the FTP server address: ", NetWorkFTPPrefix);
		std::cin >> address;
	} while (address == TRAP::Network::IPv4Address::None);

	//Connect to the server
	TRAP::Network::FTP server;
	TRAP::Network::FTP::Response connectResponse = server.Connect(address);
	fmt::println("{}{}", NetWorkFTPPrefix, connectResponse);
	if (!connectResponse.IsOK())
		return;

	//Ask for user name and password
	std::string user, password;
	fmt::print("{}User: ", NetWorkFTPPrefix);
	std::cin >> user;
	fmt::print("{}Password: ", NetWorkFTPPrefix);
	std::cin >> password;

	//Login to the server
	TRAP::Network::FTP::Response loginResponse = server.Login(user, password);
	fmt::println("{}{}: ", NetWorkFTPPrefix, loginResponse);
	if (!loginResponse.IsOK())
		return;

	//Main menu
	int32_t choice = 0;
	do
	{
		//Main FTP menu
		fmt::println("");
		fmt::println("{}Choose an action:", NetWorkFTPPrefix);
		fmt::println("{}1. Print working directory", NetWorkFTPPrefix);
		fmt::println("{}2. Print contents of working directory", NetWorkFTPPrefix);
		fmt::println("{}3. Change directory", NetWorkFTPPrefix);
		fmt::println("{}4. Create directory", NetWorkFTPPrefix);
		fmt::println("{}5. Delete directory", NetWorkFTPPrefix);
		fmt::println("{}6. Rename file", NetWorkFTPPrefix);
		fmt::println("{}7. Remove file", NetWorkFTPPrefix);
		fmt::println("{}8. Download file", NetWorkFTPPrefix);
		fmt::println("{}9. Upload file", NetWorkFTPPrefix);
		fmt::println("{}0. Disconnect", NetWorkFTPPrefix);
		fmt::println("");

		fmt::print("{}Your choice: ", NetWorkFTPPrefix);
		std::cin >> choice;
		fmt::println("");

		switch(choice)
		{
		case 1:
		{
			//Print the current server directory
			TRAP::Network::FTP::DirectoryResponse response = server.GetWorkingDirectory();
			fmt::println("{}{}", NetWorkFTPPrefix, response);
			fmt::println("{}Current directory: {}", NetWorkFTPPrefix, response.GetDirectory().string());
			break;
		}

		case 2:
		{
			//Print the contents of the current server directory
			TRAP::Network::FTP::ListingResponse response = server.GetDirectoryListing();
			fmt::println("{}{}", NetWorkFTPPrefix, response);
			const std::vector<std::filesystem::path>& names = response.GetListing();
			for (const std::filesystem::path& name : names)
				fmt::println("{}{}", NetWorkFTPPrefix, name.string());
			break;
		}

		case 3:
		{
			//Change the current directory
			std::filesystem::path directory;
			fmt::print("{}Choose a directory: ", NetWorkFTPPrefix);
			std::cin >> directory;
			fmt::println("{}{}", NetWorkFTPPrefix, server.ChangeDirectory(directory));
			break;
		}

		case 4:
		{
			//Create a new directory
			std::filesystem::path directory;
			fmt::print("{}Name of the directory to create: ", NetWorkFTPPrefix);
			std::cin >> directory;
			fmt::println("{}{}", NetWorkFTPPrefix, server.CreateDirectory(directory));
			break;
		}

		case 5:
		{
			//Remove an existing directory
			std::filesystem::path directory;
			fmt::print("{}Name of the directory to remove: ", NetWorkFTPPrefix);
			std::cin >> directory;
			fmt::println("{}{}", NetWorkFTPPrefix, server.DeleteDirectory(directory));
			break;
		}

		case 6:
		{
			//Rename a file
			std::filesystem::path source, destination;
			fmt::print("{}Name of the file to rename: ", NetWorkFTPPrefix);
			std::cin >> source;
			fmt::print("{}New name: ", NetWorkFTPPrefix);
			std::cin >> destination;
			fmt::println("{}{}", NetWorkFTPPrefix, server.RenameFile(source, destination));
			break;
		}

		case 7:
		{
			//Remove an existing file
			std::filesystem::path filename;
			fmt::print("{}Name of the file to remove: ", NetWorkFTPPrefix);
			std::cin >> filename;
			fmt::println("{}{}", NetWorkFTPPrefix, server.DeleteFile(filename));
			break;
		}

		case 8:
		{
			//Download a file from server
			std::filesystem::path filename, directory;
			fmt::print("{}Filename of the file to download (relative to current directory): ", NetWorkFTPPrefix);
			std::cin >> filename;
			fmt::print("{}Directory to download the file to: ", NetWorkFTPPrefix);
			std::cin >> directory;
			fmt::println("{}{}", NetWorkFTPPrefix, server.Download(filename, directory));
			break;
		}

		case 9:
		{
			//Upload a file to server
			std::filesystem::path filename, directory;
			fmt::print("{}Path of the file to upload (absolute or relative to working directory): ", NetWorkFTPPrefix);
			std::cin >> filename;
			fmt::print("{}Directory to upload the file to (relative to current directory): ", NetWorkFTPPrefix);
			std::cin >> directory;
			fmt::println("{}{}", NetWorkFTPPrefix, server.Upload(filename, directory));
			break;
		}

		case 0:
		{
			//Disconnect
			break;
		}

		default:
			//Wrong choice
			fmt::println("{}Invalid choice!", NetWorkFTPPrefix);
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			break;
		}
	} while (choice != 0);

	//Disconnect from the server
	fmt::println("{}Disconnecting from server...", NetWorkFTPPrefix);
	fmt::println("{}{}", NetWorkFTPPrefix, server.Disconnect());
}