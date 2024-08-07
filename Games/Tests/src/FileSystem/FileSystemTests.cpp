#include "FileSystemTests.h"

namespace
{
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event)
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
}

//-------------------------------------------------------------------------------------------------------------------//

void FileSystemTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("FileSystem");
}

//-------------------------------------------------------------------------------------------------------------------//

void FileSystemTests::OnImGuiRender()
{
	ImGui::Begin("FileSystem", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                    ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press O to open the working folder in the file browser");
	ImGui::Text("Press F to open a file in the file browser");
	ImGui::Text("Press E to externally open a file");

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void FileSystemTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}
