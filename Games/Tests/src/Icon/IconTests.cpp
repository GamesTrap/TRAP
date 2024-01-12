#include "IconTests.h"

namespace
{
	//Simple TRAP Logo
	constexpr std::array<std::array<char, 17>, 16> IconStrings
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

	constexpr std::array<std::array<u8, 4>, 5> IconColors
	{
		{
			{  0,   0,   0, 255}, //Black
			{255,   0,   0, 255}, //Red
			{  0, 255,   0, 255}, //Green
			{  0,   0, 255, 255}, //Blue
			{255, 255, 255, 255}  //White
		}
	};
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

	SetIcon(*TRAP::Application::GetWindow(), s_cursorIconColor);
}

//-------------------------------------------------------------------------------------------------------------------//

void IconTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}

//-------------------------------------------------------------------------------------------------------------------//

void IconTests::SetIcon(const TRAP::Window& window, const u32 iconColor)
{
	std::vector<u8> pixels(16ull * 16ull * 4ull, 0);
	usize target = 0;

	for(u32 y = 0; y < 16u; y++)
	{
		for(u32 x = 0; x < 16u; x++)
		{
			if (IconStrings[y][x] == '0')
				memcpy(&pixels[target], IconColors[iconColor].data(), 4);
			else
				memset(&pixels[target], 0, 4u);

			target += 4u;
		}
	}

	window.SetIcon(TRAP::Image::LoadFromMemory(16u, 16u, TRAP::Image::ColorFormat::RGBA, pixels).get());
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
		s_cursorIconColor = (s_cursorIconColor + 1) % NumericCast<u32>(IconColors.size());
		SetIcon(*TRAP::Application::GetWindow(), s_cursorIconColor);
		break;

	case TRAP::Input::Key::X:
		TRAP::Application::GetWindow()->SetIcon();
		break;

	default:
		break;
	}

	return true;
}