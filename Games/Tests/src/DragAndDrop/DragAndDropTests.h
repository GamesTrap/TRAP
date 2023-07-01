#ifndef GAMESTRAP_DRAGANDDROPTESTS_H
#define GAMESTRAP_DRAGANDDROPTESTS_H

#include <TRAP.h>

class DragAndDropTests final : public TRAP::Layer
{
public:
	DragAndDropTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnDrop(const TRAP::Events::WindowDropEvent& event);

	bool m_dragAndDrop;
	std::vector<std::string> m_dropData;
};

#endif /*GAMESTRAP_DRAGANDDROPTESTS_H*/