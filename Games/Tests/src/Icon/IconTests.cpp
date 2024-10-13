#include "IconTests.h"

namespace
{
	constinit u32 CurrentCursorIconColor = 0u;

	//Simple TRAP Logo
	constexpr std::array<std::array<char, 17u>, 16u> IconStrings
	{
		{
			{"................"},
			{"................"},
			{"..00000...000..."},
			{"....0.....0..0.."},
			{"....0.....000..."},
			{"....0.....0.0..."},
			{"....0.....0..0.."},
			{"................"},
			{"................"},
			{"....0.....000..."},
			{"...0.0....0..0.."},
			{"..0...0...000..."},
			{"..00000...0....."},
			{"..0...0...0....."},
			{"................"},
			{"................"}
		}
	};

	//-------------------------------------------------------------------------------------------------------------------//

	constexpr std::array<std::array<u8, 4u>, 5u> IconColors
	{
		{
			{  0u,   0u,   0u, 255u}, //Black
			{255u,   0u,   0u, 255u}, //Red
			{  0u, 255u,   0u, 255u}, //Green
			{  0u,   0u, 255u, 255u}, //Blue
			{255u, 255u, 255u, 255u}  //White
		}
	};

	//-------------------------------------------------------------------------------------------------------------------//

	void SetIcon(const TRAP::Window& window, const u32 iconColor)
	{
		std::vector<u8> pixels(16ull * 16ull * 4ull);
		usize target = 0u;

		for(u32 y = 0u; y < 16u; ++y)
		{
			for(u32 x = 0u; x < 16u; ++x)
			{
				if (IconStrings[y][x] == '0')
					memcpy(&pixels[target], IconColors[iconColor].data(), 4u);
				else
					memset(&pixels[target], 0, 4u);

				target += 4u;
			}
		}

		window.SetIcon(TRAP::Image::LoadFromMemory(16u, 16u, TRAP::Image::ColorFormat::RGBA, pixels).get());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void IconTests::OnImGuiRender()
{
	ImGui::Begin("Icon", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                              ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press Space to cycle through Window Icons");
	ImGui::Text("Press X to reset Window Icon");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void IconTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Icon");

	SetIcon(*TRAP::Application::GetWindow(), CurrentCursorIconColor);
}


//-------------------------------------------------------------------------------------------------------------------//

bool IconTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::Space:
		CurrentCursorIconColor = (CurrentCursorIconColor + 1u) % NumericCast<u32>(IconColors.size());
		SetIcon(*TRAP::Application::GetWindow(), CurrentCursorIconColor);
		break;

	case TRAP::Input::Key::X:
		TRAP::Application::GetWindow()->SetIcon();
		break;

	default:
		break;
	}

	return true;
}
