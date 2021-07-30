#ifndef _GAMESTRAP_DRAGANDDROPTESTS_H_
#define _GAMESTRAP_DRAGANDDROPTESTS_H_

#include <TRAP.h>

class DragAndDropTests final : public TRAP::Layer
{
public:
	DragAndDropTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	bool OnDrop(TRAP::Events::WindowDropEvent& event);

	bool m_dragAndDrop;
	std::vector<std::string> m_dropData;
};

#endif /*_GAMESTRAP_DRAGANDDROPTESTS_H_*/