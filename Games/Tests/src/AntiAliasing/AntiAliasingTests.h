#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	AntiAliasingTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer;

	TRAP::Graphics::AntiAliasing m_antiAliasing;
	TRAP::Graphics::SampleCount m_sampleCount;

	TRAP::Graphics::OrthographicCamera m_camera;

	static std::vector<TRAP::Graphics::AntiAliasing> AntiAliasingMethods;

	struct SampleData
	{
		const char* Name;
		TRAP::Graphics::SampleCount Samples;
	};

	inline static constexpr std::array<SampleData, 4> Samples
	{
		{
			{ "x2", TRAP::Graphics::SampleCount::Two },
			{ "x4", TRAP::Graphics::SampleCount::Four },
			{ "x8", TRAP::Graphics::SampleCount::Eight },
			{ "x16", TRAP::Graphics::SampleCount::Sixteen }
		}
	};
};

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/