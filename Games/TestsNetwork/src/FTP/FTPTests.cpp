#include "FTPTests.h"

std::ostream& operator<<(std::ostream& stream, const TRAP::Network::FTP::Response& response)
{
	return stream << static_cast<int32_t>(response.GetStatus()) << response.GetMessage();
}

//-------------------------------------------------------------------------------------------------------------------//

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

void FTPTests::FTP()
{
	//Choose the server address
	TRAP::Network::IPv4Address address;
	do
	{
		std::cout << "[Network][FTP] Enter the FTP server address: ";
		std::cin >> address;
	} while (address == TRAP::Network::IPv4Address::None);

	//Connect to the server
	TRAP::Network::FTP server;
	TRAP::Network::FTP::Response connectResponse = server.Connect(address);
	std::cout << "[Network][FTP] " << connectResponse << std::endl;
	if (!connectResponse.IsOK())
		return;

	//Ask for user name and password
	std::string user, password;
	std::cout << "[Network][FTP] User: ";
	std::cin >> user;
	std::cout << "[Network][FTP] Password: ";
	std::cin >> password;

	//Login to the server
	TRAP::Network::FTP::Response loginResponse = server.Login(user, password);
	std::cout << "[Network][FTP] " << loginResponse << std::endl;
	if (!loginResponse.IsOK())
		return;

	//Main menu
	int32_t choice = 0;
	do
	{
		//Main FTP menu
		std::cout << std::endl;
		std::cout << "[Network][FTP] Choose an action:" << std::endl;
		std::cout << "[Network][FTP] 1. Print working directory" << std::endl;
		std::cout << "[Network][FTP] 2. Print contents of working directory" << std::endl;
		std::cout << "[Network][FTP] 3. Change directory" << std::endl;
		std::cout << "[Network][FTP] 4. Create directory" << std::endl;
		std::cout << "[Network][FTP] 5. Delete directory" << std::endl;
		std::cout << "[Network][FTP] 6. Rename file" << std::endl;
		std::cout << "[Network][FTP] 7. Remove file" << std::endl;
		std::cout << "[Network][FTP] 8. Download file" << std::endl;
		std::cout << "[Network][FTP] 9. Upload file" << std::endl;
		std::cout << "[Network][FTP] 0. Disconnect" << std::endl;
		std::cout << std::endl;

		std::cout << "[Network][FTP] Your choice: ";
		std::cin >> choice;
		std::cout << std::endl;

		switch(choice)
		{
		case 1:
		{
			//Print the current server directory
			TRAP::Network::FTP::DirectoryResponse response = server.GetWorkingDirectory();
			std::cout << "[Network][FTP] " << response << std::endl;
			std::cout << "[Network][FTP] Current directory is " << response.GetDirectory().u8string() << std::endl;
			break;
		}

		case 2:
		{
			//Print the contents of the current server directory
			TRAP::Network::FTP::ListingResponse response = server.GetDirectoryListing();
			std::cout << "[Network][FTP] " << response << std::endl;
			const std::vector<std::filesystem::path>& names = response.GetListing();
			for (const std::filesystem::path& name : names)
				std::cout << "[Network][FTP] " << name.u8string() << std::endl;
			break;
		}

		case 3:
		{
			//Change the current directory
			std::filesystem::path directory;
			std::cout << "[Network][FTP] Choose a directory: ";
			std::cin >> directory;
			std::cout << "[Network][FTP] " << server.ChangeDirectory(directory) << std::endl;
			break;
		}

		case 4:
		{
			//Create a new directory
			std::filesystem::path directory;
			std::cout << "[Network][FTP] Name of the directory to create: ";
			std::cin >> directory;
			std::cout << "[Network][FTP] " << server.CreateDirectory(directory) << std::endl;
			break;
		}

		case 5:
		{
			//Remove an existing directory
			std::filesystem::path directory;
			std::cout << "[Network][FTP] Name of the directory to remove: ";
			std::cin >> directory;
			std::cout << "[Network][FTP] " << server.DeleteDirectory(directory) << std::endl;
			break;
		}

		case 6:
		{
			//Rename a file
			std::filesystem::path source, destination;
			std::cout << "[Network][FTP] Name of the file to rename: ";
			std::cin >> source;
			std::cout << "[Network][FTP] " << "New name: ";
			std::cin >> destination;
			std::cout << "[Network][FTP] " << server.RenameFile(source, destination) << std::endl;
			break;
		}

		case 7:
		{
			//Remove an existing file
			std::filesystem::path filename;
			std::cout << "[Network][FTP] Name of the file to remove: ";
			std::cin >> filename;
			std::cout << "[Network][FTP] " << server.DeleteFile(filename) << std::endl;
			break;
		}

		case 8:
		{
			//Download a file from server
			std::filesystem::path filename, directory;
			std::cout << "[Network][FTP] Filename of the file to download (relative to current directory): ";
			std::cin >> filename;
			std::cout << "[Network][FTP] Directory to download the file to: ";
			std::cin >> directory;
			std::cout << "[Network][FTP] " << server.Download(filename, directory) << std::endl;
			break;
		}

		case 9:
		{
			//Upload a file to server
			std::filesystem::path filename, directory;
			std::cout << "[Network][FTP] Path of the file to upload (absolute or relative to working directory): ";
			std::cin >> filename;
			std::cout << "[Network][FTP] Directory to upload the file to (relative to current directory): ";
			std::cin >> directory;
			std::cout << server.Upload(filename, directory) << std::endl;
			break;
		}

		case 0:
		{
			//Disconnect
			break;
		}

		default:
			//Wrong choice
			std::cout << "[Network][FTP] Invalid choice!" << std::endl;
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			break;
		}
	} while (choice != 0);

	//Disconnect from the server
	std::cout << "[Network][FTP] Disconnecting from server..." << std::endl;
	std::cout << "[Network][FTP] " << server.Disconnect() << std::endl;
}