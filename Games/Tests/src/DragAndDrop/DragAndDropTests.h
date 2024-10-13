#ifndef GAMESTRAP_DRAGANDDROPTESTS_H
#define GAMESTRAP_DRAGANDDROPTESTS_H

#include <TRAP.h>

class DragAndDropTests final : public TRAP::Layer
{
public:
	constexpr DragAndDropTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	constexpr bool OnDrop(const TRAP::Events::WindowDropEvent& event);

	bool m_dragAndDrop = true;
	std::vector<std::string> m_dropData{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr DragAndDropTests::DragAndDropTests()
	: Layer("DragAndDrop")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void DragAndDropTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&DragAndDropTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::WindowDropEvent>(std::bind_front(&DragAndDropTests::OnDrop, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool DragAndDropTests::OnDrop(const TRAP::Events::WindowDropEvent& event)
{
	m_dropData = event.GetPaths();

	return true;
}

#endif /*GAMESTRAP_DRAGANDDROPTESTS_H*/
