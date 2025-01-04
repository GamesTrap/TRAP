#include "FileSystemTests.h"

void FileSystemTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("FileSystem");
}

//-------------------------------------------------------------------------------------------------------------------//

void FileSystemTests::OnImGuiRender()
{
	ImGui::Begin("FileSystem", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                    ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::TextFmt("Press ESC to close");
	ImGui::TextFmt("Press O to open the working folder in the file browser");
	ImGui::TextFmt("Press F to open a file in the file browser");
	ImGui::TextFmt("Press E to externally open a file");

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool FileSystemTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::O:
	{
		const auto currentFolder = TRAP::FileSystem::GetCurrentWorkingFolderPath();
		if(currentFolder)
			TRAP::FileSystem::OpenInFileBrowser(*currentFolder);
		break;
	}

	case TRAP::Input::Key::F:
		TRAP::FileSystem::OpenInFileBrowser("Assets/Textures/vulkanlogo.png");
		break;

	case TRAP::Input::Key::E:
		TRAP::FileSystem::OpenExternally("Assets/Textures/vulkanlogo.png");
		break;

	default:
		break;
	}

	return true;
}
