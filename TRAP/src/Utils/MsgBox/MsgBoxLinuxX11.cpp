#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_LINUX

#include "MsgBoxLinuxX11.h"
#include "MsgBox.h"

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::MsgBox::INTERNAL::X11::DrawButton(Button* b, Display* dpy, ::Window w, GC gc)
{
	if (b->Mouseover)
	{
		XSetForeground(dpy, gc, ButtonBackgroundColor);
		XSetBackground(dpy, gc, ButtonBorderColor);
		XFillRectangle(dpy, w, gc, b->Clicked + b->X, b->Clicked + b->Y, b->Width, b->Height);
	}
	else
	{
		XSetForeground(dpy, gc, ButtonBorderColor);
		XSetBackground(dpy, gc, ButtonBackgroundColor);
		XDrawRectangle(dpy, w, gc, b->X, b->Y, b->Width, b->Height);
	}

	XSetForeground(dpy, gc, TextColor);
	XSetBackground(dpy, gc, ButtonBackgroundColor);
	XDrawString(dpy, w, gc, b->Clicked + b->TextX, b->Clicked + b->TextY, b->Text, std::strlen(b->Text));
}

//-------------------------------------------------------------------------------------------------------------------//

int TRAP::Utils::MsgBox::INTERNAL::X11::IsPointInside(Button* b, const int px, const int py)
{
	return px >= b->X && px <= (b->X + static_cast<int>(b->Width) - 1) && py >= b->Y && py <= (b->Y + static_cast<int>(b->Height) - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::MsgBox::INTERNAL::X11::Cleanup(Display* display, ::Window& window, GC& gc)
{
	XFreeGC(display, gc);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::MsgBox::INTERNAL::X11::SingleButtonMsgBoxEventLoop(Display* display, ::Window& window, Button& btn, const char* message, int& height, GC& gc)
{
	XEvent event;
	const char* temp, * end;
	std::size_t i;
	char* atom;
	const char* wmDeleteWindow = "WM_DELETE_WINDOW";

	//Event Loop
	while (true)
	{
		XNextEvent(display, &event);
		btn.Clicked = false;

		if (event.type == MotionNotify)
		{
			if (X11::IsPointInside(&btn, event.xmotion.x, event.xmotion.y))
			{
				if (!btn.Mouseover)
					event.type = Expose;

				btn.Mouseover = true;
			}
			else
			{
				if (btn.Mouseover)
					event.type = Expose;

				btn.Mouseover = false;
				btn.Clicked = false;
			}
		}

		switch (event.type)
		{
		case ButtonPress:
		case ButtonRelease:
			if (event.xbutton.button != Button1)
				break;

			if (btn.Mouseover)
			{
				btn.Clicked = event.type == ButtonPress ? true : false;

				if (!btn.Clicked)
					return;
			}
			else
				btn.Clicked = false;
			break;

		case Expose:
		case MapNotify:
			XClearWindow(display, window);

			//Draw text lines
			for (i = 0, temp = message; temp; temp = end ? (end + 1) : nullptr, i += height)
			{
				end = std::strchr(temp, '\n');

				XDrawString(display, window, gc, 10, 10 + height + i, temp, end ? static_cast<unsigned int>(end - temp) : std::strlen(temp));
			}

			//Draw Button
			X11::DrawButton(&btn, display, window, gc);
			XFlush(display);
			break;

		case 3:
			if (XLookupKeysym(&event.xkey, 0) == XK_Escape || XLookupKeysym(&event.xkey, 0) == XK_Return || XLookupKeysym(&event.xkey, 0) == XK_KP_Enter)
				return;
			break;

		case ClientMessage:
			atom = XGetAtomName(display, event.xclient.message_type);

			if (*atom == *wmDeleteWindow)
			{
				XFree(atom);

				return;
			}

			XFree(atom);
			break;

		default:
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::INTERNAL::X11::DoubleButtonMsgBoxEventLoop(Display* display, ::Window& window, Button& btn1, Button& btn2, const char* message, int& height, GC& gc)
{
	XEvent event;
	const char* temp, * end;
	std::size_t i;
	char* atom;
	const char* wmDeleteWindow = "WM_DELETE_WINDOW";

	//Event Loop
	while (true)
	{
		XNextEvent(display, &event);
		btn1.Clicked = false;
		btn2.Clicked = false;

		if (event.type == MotionNotify)
		{
			if (X11::IsPointInside(&btn1, event.xmotion.x, event.xmotion.y))
			{
				if (!btn1.Mouseover)
					event.type = Expose;

				btn1.Mouseover = true;
			}
			else if (X11::IsPointInside(&btn2, event.xmotion.x, event.xmotion.y))
			{
				if (!btn2.Mouseover)
					event.type = Expose;

				btn2.Mouseover = true;
			}
			else
			{
				if (btn1.Mouseover)
					event.type = Expose;
				if (btn2.Mouseover)
					event.type = Expose;

				btn1.Mouseover = false;
				btn1.Clicked = false;
				btn2.Mouseover = false;
				btn2.Clicked = false;
			}
		}

		switch (event.type)
		{
		case ButtonPress:
		case ButtonRelease:
			if (event.xbutton.button != Button1)
				break;

			if (btn1.Mouseover)
			{
				btn1.Clicked = event.type == ButtonPress ? true : false;

				if (!btn1.Clicked)
				{
					if (!std::strcmp(btn1.Text, "Yes"))
						return Selection::Yes;
					if (!std::strcmp(btn2.Text, "OK"))
						return Selection::OK;

					return Selection::Error;
				}
			}
			else if (btn2.Mouseover)
			{
				btn2.Clicked = event.type == ButtonPress ? true : false;

				if (!btn2.Clicked)
				{
					if (!std::strcmp(btn1.Text, "No"))
						return Selection::No;
					if (!std::strcmp(btn2.Text, "Cancel"))
						return Selection::Cancel;

					return Selection::Error;
				}
			}
			else
			{
				btn1.Clicked = false;
				btn2.Clicked = false;
			}
			break;

		case Expose:
		case MapNotify:
			XClearWindow(display, window);

			//Draw text lines
			for (i = 0, temp = message; temp; temp = end ? (end + 1) : nullptr, i += height)
			{
				end = std::strchr(temp, '\n');

				XDrawString(display, window, gc, 10, 10 + height + i, temp, end ? static_cast<unsigned int>(end - temp) : std::strlen(temp));
			}

			//Draw Buttons
			X11::DrawButton(&btn1, display, window, gc);
			X11::DrawButton(&btn2, display, window, gc);
			XFlush(display);
			break;

		case 3:
			if (XLookupKeysym(&event.xkey, 0) == XK_Escape || XLookupKeysym(&event.xkey, 0) == XK_Return || XLookupKeysym(&event.xkey, 0) == XK_KP_Enter)
			{
				if (!std::strcmp(btn1.Text, "No"))
					return Selection::No;
				if (!std::strcmp(btn2.Text, "Cancel"))
					return Selection::Cancel;

				return Selection::Error;
			}
			break;

		case ClientMessage:
			atom = XGetAtomName(display, event.xclient.message_type);

			if (*atom == *wmDeleteWindow)
			{
				XFree(atom);

				if (!std::strcmp(btn1.Text, "No"))
					return Selection::No;
				if (!std::strcmp(btn2.Text, "Cancel"))
					return Selection::Cancel;

				return Selection::Error;
			}

			XFree(atom);
			break;

		default:
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::MsgBox::INTERNAL::X11::SingleButtonMsgBox(const char* btnName,
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
	                                                        Button& btn)
{
	//Compute the shape of the first button
	XTextExtents(font, btnName, 2, &direction, &ascent, &descent, &overall);

	btn.Width = overall.width + 30;
	btn.Height = ascent + descent + 5;

	btn.X = static_cast<int>(W / 2 - btn.Width / 2);
	btn.Y = H - height - 15;
	if (!std::strcmp(btnName, "Quit"))
		btn.TextX = btn.X + 10;
	else
		btn.TextX = btn.X + 15;
	btn.TextY = static_cast<int>(btn.Y + btn.Height - 3);
	btn.Mouseover = false;
	btn.Clicked = false;
	btn.Text = btnName;

	//Don't need that anymore
	XFreeFontInfo(NULL, font, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::MsgBox::INTERNAL::X11::DoubleButtonMsgBox(const char* btnName1,
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
	                                                        Button& btn2)
{
	//Compute the shape of the first button
	XTextExtents(font, btnName1, 2, &direction, &ascent, &descent, &overall);

	btn1.Width = overall.width + 30;
	btn1.Height = ascent + descent + 5;
	btn1.X = static_cast<int>(W / 4 - btn1.Width / 2);
	btn1.Y = H - height - 15;
	btn1.TextX = btn1.X + 15;
	btn1.TextY = static_cast<int>(btn1.Y + btn1.Height - 3);
	btn1.Mouseover = false;
	btn1.Clicked = false;
	btn1.Text = btnName1;

	//Compute the shape of the second button
	XTextExtents(font, btnName2, 2, &direction, &ascent, &descent, &overall);

	btn2.Width = overall.width + 30;
	btn2.Height = ascent + descent + 5;
	btn2.X = static_cast<int>(W / 2 + btn2.Width / 2);
	btn2.Y = H - height - 15;
	if (!std::strcmp(btnName2, "Cancel"))
		btn2.TextX = btn2.X + 5;
	else
		btn2.TextX = btn2.X + 15;
	btn2.TextY = static_cast<int>(btn2.Y + btn2.Height - 3);
	btn2.Mouseover = false;
	btn2.Clicked = false;
	btn2.Text = btnName2;

	//Don't need that anymore
	XFreeFontInfo(NULL, font, 1);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::INTERNAL::X11::ShowX11(const char* message,
	                                                                       const char* title,
	                                                                       const Style style,
	                                                                       const Buttons buttons)
{
	//Style gets ignored because there are no icons to choose from
	Display* display;
	Atom wmDelete;
	XFontStruct* font;
	const char* end;
	std::size_t lines = 0;
	int direction = 0, ascent = 0, descent = 0, height = 0, W = 0, H = 0;
	XCharStruct overall;
	XSizeHints hints;

	//Open a display
	if (!((display = XOpenDisplay(0))))
	{
		TP_ERROR("[MsgBox][X11] Couldn't open display!");
		return Selection::Error;
	}

	//Create a window with the specified title
	::Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 100, 100, 0, BackgroundColor, BackgroundColor);

	XSelectInput(display, window, ExposureMask | StructureNotifyMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	XMapWindow(display, window);
	XStoreName(display, window, title);

	wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
	XSetWMProtocols(display, window, &wmDelete, 1);

	//Make window always on top
	Atom wmStateAbove = XInternAtom(display, "_NET_WM_STATE_ABOVE", 1);
	Atom wmNetWmState = XInternAtom(display, "_NET_WM_STATE", 1);
	if (wmStateAbove != 0)
	{
		XClientMessageEvent xclient{};
		xclient.type = ClientMessage;
		xclient.window = window;
		xclient.message_type = wmNetWmState;
		xclient.format = 32;
		xclient.data.l[0] = 1;
		xclient.data.l[1] = wmStateAbove;

		XSendEvent(display, DefaultRootWindow(display), false, SubstructureRedirectMask | SubstructureNotifyMask, reinterpret_cast<XEvent*>(&xclient));
		XFlush(display);
	}

	//Set Title
	XClassHint* hint = XAllocClassHint();
	hint->res_name = const_cast<char*>(title);
	hint->res_class = const_cast<char*>(title);
	XSetClassHint(display, window, hint);
	XFree(hint);

	//Create a graphics context for the window
	GC gc = XCreateGC(display, window, 0, 0);

	XSetForeground(display, gc, BackgroundColor);

	//Compute the printed width and height of the text
	if (!((font = XQueryFont(display, XGContextFromGC(gc)))))
	{
		X11::Cleanup(display, window, gc);
		return TRAP::Utils::MsgBox::Selection::Error;
	}

	for (const char* temp = message; temp; temp = end ? (end + 1) : nullptr, ++lines)
	{
		end = std::strchr(temp, '\n');
		XTextExtents(font, temp, end ? static_cast<unsigned int>(end - temp) : std::strlen(temp), & direction, & ascent, & descent, & overall);

		W = overall.width > W ? overall.width : W;
		height = (ascent + descent) > height ? (ascent + descent) : height;
	}

	//Compute the shape of the window and adjust the window accordingly
	W += 20;
	H = static_cast<int>(lines)* height + height + 40;
	int X = DisplayWidth(display, DefaultScreen(display)) / 2 - W / 2;
	int Y = DisplayHeight(display, DefaultScreen(display)) / 2 - H / 2;

	XMoveResizeWindow(display, window, X, Y, W, H);

	//Make the window non resizable
	XUnmapWindow(display, window);

	hints.flags = PSize | PMinSize | PMaxSize;
	hints.min_width = hints.max_width = hints.base_width = W;
	hints.min_height = hints.max_height = hints.base_height = H;

	XSetWMNormalHints(display, window, &hints);
	XMapRaised(display, window);
	XFlush(display);

	if (buttons == Buttons::OK)
	{
		Button ok;
		X11::SingleButtonMsgBox("OK", font, direction, ascent, descent, overall, W, H, height, X, Y, ok);
		X11::SingleButtonMsgBoxEventLoop(display, window, ok, message, height, gc);

		X11::Cleanup(display, window, gc);

		return Selection::OK;
	}
	if (buttons == Buttons::Quit)
	{
		Button quit;
		X11::SingleButtonMsgBox("Quit", font, direction, ascent, descent, overall, W, H, height, X, Y, quit);
		X11::SingleButtonMsgBoxEventLoop(display, window, quit, message, height, gc);

		X11::Cleanup(display, window, gc);

		return Selection::Quit;
	}
	if (buttons == Buttons::YesNo)
	{
		Button yes;
		Button no;
		X11::DoubleButtonMsgBox("Yes", "No", font, direction, ascent, descent, overall, W, H, height, X, Y, yes, no);
		const Selection selection = INTERNAL::X11::DoubleButtonMsgBoxEventLoop(display, window, yes, no, message, height, gc);

		X11::Cleanup(display, window, gc);

		return selection;
	}
	if (buttons == Buttons::OKCancel)
	{
		Button ok;
		Button cancel;
		X11::DoubleButtonMsgBox("OK", "Cancel", font, direction, ascent, descent, overall, W, H, height, X, Y, ok, cancel);
		Selection selection = INTERNAL::X11::DoubleButtonMsgBoxEventLoop(display, window, ok, cancel, message, height, gc);

		X11::Cleanup(display, window, gc);

		return selection;
	}

	//Shouldn't be reached normally
	XFreeFontInfo(NULL, font, 1);
	X11::Cleanup(display, window, gc);

	return Selection::Error;
}

#endif