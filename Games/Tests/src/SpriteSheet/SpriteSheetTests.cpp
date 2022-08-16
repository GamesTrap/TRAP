#include "SpriteSheetTests.h"
#include "Graphics/Textures/SpriteManager.h"

SpriteSheetTests::SpriteSheetTests()
	: Layer("Sprite Sheet Tests"),
	m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void SpriteSheetTests::OnImGuiRender()
{
	TP_PROFILE_FUNCTION();

    ImGui::Begin("Sprite Sheet Tests", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                                         ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void SpriteSheetTests::OnAttach()
{
	TP_PROFILE_FUNCTION();

	TRAP::Application::GetWindow()->SetTitle("Sprite Sheet Tests");

	//Load Textures
	m_backgroundSheet = TRAP::Graphics::TextureManager::Load("BackgroundSheet", "./Assets/Textures/SpriteSheets/background_packed.png");
	m_foregroundSheet = TRAP::Graphics::TextureManager::Load("ForegroundSheet", "./Assets/Textures/SpriteSheets/foreground_packed.png");
	m_characterSheet = TRAP::Graphics::TextureManager::Load("CharacterSheet", "./Assets/Textures/SpriteSheets/characters_packed.png");
	m_backgroundSheet->AwaitLoading();
	m_foregroundSheet->AwaitLoading();
	m_characterSheet->AwaitLoading();

	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
	TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha, TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);

    constexpr TRAP::Math::Vec2 foregroundSpriteSize{18.0f, 18.0f};
    constexpr TRAP::Math::Vec2 backgroundSpriteSize{24.0f, 24.0f};
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthBorderLeft", m_foregroundSheet, {1.0f, 6.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthBorderLeftBottom", m_foregroundSheet, {1.0f, 7.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthBorderBottom", m_foregroundSheet, {2.0f, 7.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthBorderRightBottom", m_foregroundSheet, {3.0f, 7.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthBorderRight", m_foregroundSheet, {3.0f, 6.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthNoBorder", m_foregroundSheet, {2.0f, 6.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthCornerRightBottom", m_foregroundSheet, {4.0f, 0.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthCornerRightTop", m_foregroundSheet, {4.0f, 1.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthCornerLeftBottom", m_foregroundSheet, {5.0f, 0.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("EarthCornerLeftTop", m_foregroundSheet, {5.0f, 1.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("GrassBorderLeftTop", m_foregroundSheet, {1.0f, 1.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("GrassBorderTop", m_foregroundSheet, {2.0f, 1.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("GrassBorderRightTop", m_foregroundSheet, {3.0f, 1.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("PlantSmall", m_foregroundSheet, {4.0f, 6.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("PlantBig", m_foregroundSheet, {5.0f, 6.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("SignRight", m_foregroundSheet, {8.0f, 4.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("MushroomSmall", m_foregroundSheet, {8.0f, 6.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("MushroomBig", m_foregroundSheet, {9.0f, 6.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("RopeTop", m_foregroundSheet, {9.0f, 3.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("RopeMiddle", m_foregroundSheet, {9.0f, 4.0f}, foregroundSpriteSize);
    TRAP::Graphics::SpriteManager::CreateFromCoords("RopeBottom", m_foregroundSheet, {9.0f, 5.0f}, foregroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("BackgroundBlue", m_backgroundSheet, {0.0f, 0.0f}, backgroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("BackgroundCloud", m_backgroundSheet, {1.0f, 0.0f}, backgroundSpriteSize);
	TRAP::Graphics::SpriteManager::CreateFromCoords("BackgroundWhite", m_backgroundSheet, {2.0f, 0.0f}, backgroundSpriteSize);

    m_backgroundMap =
    {
        {{"EarthNoBorder", "EarthBorderRight", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthBorderRight"}},
        {{"EarthNoBorder", "EarthBorderRight", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthBorderRight"}},
        {{"EarthBorderBottom", "EarthBorderRightBottom", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthCornerRightBottom", "EarthBorderRightBottom"}},
        {{"EarthNoBorder", "EarthNoBorder", "EarthCornerRightBottom", "EarthBorderBottom", "EarthBorderBottom", "EarthBorderRightBottom", "BackgroundBlue"}},
        {{"EarthNoBorder", "EarthNoBorder", "EarthBorderRight", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue"}},
        {{"EarthBorderBottom", "EarthBorderBottom", "EarthBorderRightBottom", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue"}},
        {{"BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue", "BackgroundBlue"}},
        {{"BackgroundCloud", "BackgroundCloud", "BackgroundCloud", "BackgroundCloud", "BackgroundCloud", "BackgroundCloud", "BackgroundCloud"}},
        {{"BackgroundWhite", "BackgroundWhite", "GrassBorderLeftTop", "GrassBorderTop", "GrassBorderTop", "GrassBorderTop", "GrassBorderRightTop"}},
        {{"GrassBorderTop", "GrassBorderTop", "EarthCornerLeftTop", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthBorderRight"}},
        {{"EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder", "EarthCornerRightTop"}},
        {{"EarthNoBorder", "GrassBorderLeftTop", "GrassBorderTop", "GrassBorderRightTop", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder"}},
        {{"EarthNoBorder", "EarthBorderLeft", "EarthNoBorder", "EarthBorderRight", "EarthNoBorder", "EarthNoBorder", "EarthNoBorder"}},
        {{"EarthNoBorder", "EarthBorderLeft", "EarthNoBorder", "GrassBorderLeftTop", "GrassBorderTop", "GrassBorderTop", "GrassBorderRightTop"}},
        {{"EarthNoBorder", "EarthBorderLeft", "EarthNoBorder", "EarthBorderLeft", "EarthNoBorder", "EarthNoBorder", "EarthBorderRight"}}
    };

    m_foregroundMap =
    {
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "RopeTop", "", ""}},
        {{"", "", "", "", "RopeMiddle", "", ""}},
        {{"", "", "", "", "RopeMiddle", "", ""}},
        {{"", "", "MushroomBig", "", "RopeBottom", "", "SignRight"}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "MushroomSmall", "", "", "", ""}},
        {{"", "", "", "", "", "", ""}},
        {{"", "", "", "", "", "PlantBig", ""}},
    };
}

//-------------------------------------------------------------------------------------------------------------------//

void SpriteSheetTests::OnDetach()
{
	TP_PROFILE_FUNCTION();

    m_backgroundSheet.reset();
    m_foregroundSheet.reset();
    m_characterSheet.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void SpriteSheetTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	TP_PROFILE_FUNCTION();

	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

	TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
	{
        //Render background sprites
        for(std::size_t y = 0; y < m_backgroundMap.size(); ++y)
        {
            for(std::size_t x = 0; x < m_backgroundMap[y].size(); ++x)
            {
                if(m_backgroundMap[y][x].empty())
                    continue;
		        TRAP::Graphics::Renderer2D::DrawQuad({ {static_cast<float>(x), -static_cast<float>(y), 0.0f}, {}, {1.0f, 1.0f, 1.0f} }, TRAP::Graphics::SpriteManager::Get(m_backgroundMap[y][x]));
            }
        }

        //Render foreground sprites
        for(std::size_t y = 0; y < m_foregroundMap.size(); ++y)
        {
            for(std::size_t x = 0; x < m_foregroundMap[y].size(); ++x)
            {
                if(m_foregroundMap[y][x].empty())
                    continue;
		        TRAP::Graphics::Renderer2D::DrawQuad({ {static_cast<float>(x), -static_cast<float>(y), 0.0f}, {}, {1.0f, 1.0f, 1.0f} }, TRAP::Graphics::SpriteManager::Get(m_foregroundMap[y][x]));
            }
        }
	}
	TRAP::Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void SpriteSheetTests::OnEvent(TRAP::Events::Event& event)
{
	m_cameraController.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool SpriteSheetTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
    if (event.GetKey() == TRAP::Input::Key::Escape && event.GetRepeatCount() < 1)
        TRAP::Application::Shutdown();

    return true;
}
