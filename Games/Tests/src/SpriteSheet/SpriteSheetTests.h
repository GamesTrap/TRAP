#ifndef GAMESTRAP_SPRITESHEETTESTS_H
#define GAMESTRAP_SPRITESHEETTESTS_H

#include <TRAP.h>

class SpriteSheetTests final : public TRAP::Layer
{
public:
	SpriteSheetTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Ref<TRAP::Graphics::Texture> m_backgroundSheet;
	TRAP::Ref<TRAP::Graphics::Texture> m_foregroundSheet;
	TRAP::Ref<TRAP::Graphics::Texture> m_characterSheet;

    std::vector<std::vector<std::string>> m_backgroundMap;
    std::vector<std::vector<std::string>> m_foregroundMap;

	TRAP::Ref<TRAP::Graphics::SpriteAnimation> m_animation;
	TRAP::Ref<TRAP::Graphics::SpriteAnimation> m_animation2;
};

#endif /*GAMESTRAP_SPRITESHEETTESTS_H*/