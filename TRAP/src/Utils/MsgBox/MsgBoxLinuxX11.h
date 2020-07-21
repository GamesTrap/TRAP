#ifndef _TRAP_MSGBOXLINUXX11_H_
#define _TRAP_MSGBOXLINUXX11_H_

#ifdef TRAP_PLATFORM_LINUX

namespace TRAP::Utils::MsgBox
{
	enum class Selection;
	enum class Style;
	enum class Buttons;
}

namespace TRAP::Utils::MsgBox::INTERNAL::X11
{
	struct Button
	{
		int X = 0, Y = 0;
		unsigned int Width = 0, Height = 0;
		int TextX = 0, TextY = 0;
		bool Mouseover = false;
		bool Clicked = false;
		const char* Text = nullptr;
	};

	constexpr uint32_t MakeRGB(uint8_t R, uint8_t G, uint8_t B)
	{
		return (static_cast<uint32_t>(R) << 16) | (static_cast<uint32_t>(G) << 8) | (static_cast<uint32_t>(B));
	}
	constexpr uint32_t BackgroundColor = MakeRGB(56, 54, 53);
	constexpr uint32_t TextColor = MakeRGB(209, 207, 205);
	constexpr uint32_t ButtonBorderColor = MakeRGB(140, 135, 129);
	constexpr uint32_t ButtonBackgroundColor = MakeRGB(105, 102, 99);

	//-------------------------------------------------------------------------------------------------------------------//

	static void DrawButton(Button* b, Display* dpy, ::Window w, GC gc);

	//-------------------------------------------------------------------------------------------------------------------//

	static int IsPointInside(Button* b, const int px, const int py);

	//-------------------------------------------------------------------------------------------------------------------//

	static void Cleanup(Display* display, ::Window& window, GC& gc);

	//-------------------------------------------------------------------------------------------------------------------//

	static void SingleButtonMsgBoxEventLoop(Display* display, ::Window& window, Button& btn, const char* message, int& height, GC& gc);

	//-------------------------------------------------------------------------------------------------------------------//

	static Selection DoubleButtonMsgBoxEventLoop(Display* display, ::Window& window, Button& btn1, Button& btn2, const char* message, int& height, GC& gc);

	//-------------------------------------------------------------------------------------------------------------------//

	static void SingleButtonMsgBox(const char* btnName,
		                           XFontStruct* font,
		                           int& direction,
		                           int& ascent,
		                           int& descent,
		                           XCharStruct& overall,
		                           int& W,
		                           int& H,
		                           int& height,
		                           int& X,
		                           int& Y,
		                           Button& btn);

	//-------------------------------------------------------------------------------------------------------------------//

	static void DoubleButtonMsgBox(const char* btnName1,
		                           const char* btnName2,
		                           XFontStruct* font,
		                           int& direction,
		                           int& ascent,
		                           int& descent,
		                           XCharStruct& overall,
		                           int& W,
		                           int& H,
		                           int& height,
		                           int& X,
		                           int& Y,
		                           Button& btn1,
		                           Button& btn2);

	TRAP::Utils::MsgBox::Selection ShowX11(const char* message,
		                                   const char* title,
		                                   const TRAP::Utils::MsgBox::Style style,
		                                   const TRAP::Utils::MsgBox::Buttons buttons);
}

#endif

#endif