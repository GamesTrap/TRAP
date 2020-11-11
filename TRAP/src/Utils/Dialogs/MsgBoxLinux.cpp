#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_LINUX

#include "MsgBox.h"
#include "Application.h"

namespace INTERNAL
{

	bool DetectPresence(const std::string& executable)
	{
		std::string buffer;
		buffer.resize(1024);
		FILE* in;

		std::string testedString = "which " + executable + " 2>/dev/null ";
		in = popen(testedString.data(), "r");
		if ((fgets(buffer.data(), buffer.size(), in) != nullptr)
			&& (buffer.find_first_of(':') == std::string::npos) && (buffer.find("no ") == std::string::npos))
		{
			pclose(in);
			return true;
		}
		else
		{
			pclose(in);
			return false;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool IsDarwin()
	{
		static int32_t isDarwin = -1;
		struct utsname lUtsname;

		if (isDarwin < 0)
			isDarwin = !uname(&lUtsname) && std::string(lUtsname.sysname) == "Darwin";

		return isDarwin;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool GetEnvDISPLAY()
	{
		return std::getenv("DISPLAY");
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool GraphicMode()
	{
		return (GetEnvDISPLAY() || (IsDarwin() && GetEnvDISPLAY()));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool ZenityPresent()
	{
		static int32_t zenityPresent = -1;

		if (zenityPresent < 0)
			zenityPresent = DetectPresence("zenity");

		return zenityPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	int32_t Zenity3Present()
	{
		static int32_t zenity3Present = -1;
		FILE* in;
		std::string buffer;
		buffer.resize(1024);

		if (zenity3Present < 0)
		{
			zenity3Present = 0;
			if (ZenityPresent())
			{
				in = popen("zenity --version", "r");
				if (fgets(buffer.data(), buffer.size(), in) != nullptr)
				{
					if (std::stoi(buffer) >= 3)
					{
						zenity3Present = 3;
						int32_t temp = std::stoi(buffer.substr(buffer.find_first_not_of('.') + 2));
						if (temp >= 18)
							zenity3Present = 5;
						else if (temp >= 10)
							zenity3Present = 4;
					}
					else if ((std::stoi(buffer) == 2) && (std::stoi(buffer.substr(buffer.find_first_not_of('.') + 2)) >= 32))
						zenity3Present == 2;
				}
				pclose(in);
			}
		}

		return GraphicMode() ? zenity3Present : 0;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool XPropPresent()
	{
		static int32_t xpropPresent = -1;

		if (xpropPresent < 0)
			xpropPresent = DetectPresence("xprop");

		return xpropPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool MateDialogPresent()
	{
		static int32_t matedialogPresent = -1;

		if (matedialogPresent < 0)
			matedialogPresent = DetectPresence("matedialog");

		return matedialogPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool ShellementaryPresent()
	{
		static int32_t shellementaryPresent = -1;

		if (shellementaryPresent < 0)
			shellementaryPresent = DetectPresence("shellementary");

		return shellementaryPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool QarmaPresent()
	{
		static int32_t qarmaPresent = -1;

		if (qarmaPresent < 0)
			qarmaPresent = DetectPresence("qarma");

		return qarmaPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool YadPresent()
	{
		static int32_t yadPresent = -1;

		if (yadPresent < 0)
			yadPresent = DetectPresence("yad");

		return yadPresent && GraphicMode();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	int32_t KDialogPresent()
	{
		static int32_t kdialogPresent = -1;
		std::string buffer;
		buffer.resize(1024);
		FILE* in;
		std::string desktop;

		if (kdialogPresent < 0)
		{
			if (ZenityPresent())
			{
				auto desktopEnv = std::getenv("XDG_SESSION_DESKTOP");
				if (!desktopEnv)
				{
					desktopEnv = std::getenv("XDG_CURRENT_DESKTOP");
					if (!desktopEnv)
					{
						desktopEnv = std::getenv("DESKTOP_SESSION");

						if (!desktopEnv)
						{
							kdialogPresent = 0;
							return kdialogPresent;
						}
					}
				}
				desktop = std::string(desktopEnv);
				if (desktop.empty() || ((desktop != "KDE" || desktop != "kde") && (desktop != "lxqt" || desktop != "LXQT")))
				{
					kdialogPresent = 0;
					return kdialogPresent;
				}
			}

			kdialogPresent = DetectPresence("kdialog");
			if (kdialogPresent)
			{
				in = popen("kdialog --attach 2>&1", "r");
				if (fgets(buffer.data(), buffer.size(), in) != nullptr)
				{
					if (buffer.find("Unknown") == std::string::npos)
						kdialogPresent = 2;
				}
				pclose(in);
			}
		}

		return GraphicMode() ? kdialogPresent : 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::MsgBox::Selection TRAP::Utils::Dialogs::MsgBox::Show(const char* message,
	const char* title,
	const Style style,
	const Buttons buttons)
{
	std::string dialogString;
	std::string tit;
	std::string msg;
	if(title)
		tit = title;
	if(message)
		msg = message;
	std::string buffer;
	buffer.resize(1024);
	Selection result = Selection::None;

	if (tit.empty() || tit.find('"') != std::string::npos)
		tit = "EMPTY TITLE";
	if (msg.empty() || msg.find('"') != std::string::npos)
		msg = "EMPTY MESSAGE";

	if (INTERNAL::KDialogPresent())
	{
		dialogString = "kdialog";
		if (INTERNAL::KDialogPresent() == 2 && INTERNAL::XPropPresent())
			dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";

		dialogString += " --";
		if (buttons == Buttons::OKCancel || buttons == Buttons::YesNo)
		{
			if (style == Style::Warning || style == Style::Error)
				dialogString += "warning";
            dialogString += "yesno";
		}
		else if (style == Style::Error)
			dialogString += "error";
		else if (style == Style::Warning)
			dialogString += "sorry";
		else
			dialogString += "msgbox";
		dialogString += " \"";
		if (!msg.empty())
			dialogString += msg;
		dialogString += "\"";
		if (buttons == Buttons::OKCancel)
			dialogString += " --yes-label OK --no-label Cancel";
        if (buttons == Buttons::Quit)
            dialogString += " --ok-label Quit";
		if (!tit.empty())
			dialogString += " --title \"" + tit + "\"";

		dialogString += ";if [ $? = 0 ];then echo 1;else echo 0;fi";
	}
    else if(INTERNAL::ZenityPresent() || INTERNAL::MateDialogPresent() || INTERNAL::ShellementaryPresent() || INTERNAL::QarmaPresent())
    {
        if(INTERNAL::ZenityPresent())
        {
            dialogString = "szAnswer=$(zenity";
            if(INTERNAL::Zenity3Present() >= 4 && INTERNAL::XPropPresent())
                dialogString += " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
        }
        else if(INTERNAL::MateDialogPresent())
            dialogString = "szAnswer=$(matedialog";
        else if(INTERNAL::ShellementaryPresent())
            dialogString = "szAnswer=$(shellementary";
        else
        {
            dialogString = "szAnswer=$(qarma";
            if(INTERNAL::XPropPresent())
                dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
        }
        dialogString += " --";
        
        if(buttons == Buttons::OKCancel)
            dialogString += "question --ok-label=OK --cancel-label=Cancel";
        else if(buttons == Buttons::YesNo)
            dialogString += "question";
        else if(style == Style::Error)
            dialogString += "error";
        else if(style == Style::Warning)
            dialogString += "warning";
        else
            dialogString += "info";
            
        if(buttons == Buttons::Quit)
            dialogString += " --ok-label=Quit";
            
        if(!tit.empty())
            dialogString += " --title=\"" + tit + "\"";
        if(!msg.empty())
            dialogString += " --text=\"" + msg + "\"";
            
        if(INTERNAL::Zenity3Present() >= 3 || (!INTERNAL::ZenityPresent() && (INTERNAL::ShellementaryPresent() || INTERNAL::QarmaPresent())))
        {
            dialogString += " --icon-name=dialog-";
            if(style == Style::Question)
                dialogString += "question";
            else if(style == Style::Error)
                dialogString += "error";
            else if(style == Style::Warning)
                dialogString += "warning";
            else
                dialogString += "information";
        }
        
        dialogString += " 2>/dev/null ";
        dialogString += ");if [ $? = 0 ];then echo 1;else echo 0;fi";
    }
    else if(INTERNAL::YadPresent())
    {
        dialogString += "szAnswer=$(yad --";
        if(buttons == Buttons::OK)
            dialogString += "button=OK:1";
        else if(buttons == Buttons::OKCancel)
            dialogString += "button=OK:1 --button=Cancel:0";
        else if(buttons == Buttons::YesNo)
            dialogString += "button=Yes:1 --button=No:0";
        else if(style == Style::Error)
            dialogString += "error";
        else if(style == Style::Warning)
            dialogString += "warning";
        else if(style == Style::Question)
            dialogString += "question";
        else
            dialogString += "info";
            
        if(!tit.empty())
            dialogString += " --title=\"" + tit + "\"";
        if(!msg.empty())
            dialogString += " --text=\"" + msg + "\"";
            
        dialogString += " 2>/dev/null ";
        dialogString += ");echo $?";
    }

	FILE* in;
	if (!(in = popen(dialogString.data(), "r")))
		return Selection::Error;
	while(fgets(buffer.data(), buffer.size(), in) != nullptr)
	{}
	pclose(in);
	uint32_t off = 0;
	for (const auto& c : buffer)
	{
		if (c == '\0')
			break;
		off++;
	}
	buffer.resize(off);
	if (buffer[buffer.size() - 1] == '\n')
		buffer.pop_back();

	if (buffer == "1")
	{
		if (buttons == Buttons::YesNo)
			result = Selection::Yes;
		else if (buttons == Buttons::OKCancel)
			result = Selection::OK;
		else if (buttons == Buttons::OK)
			result = Selection::OK;
        else if (buttons == Buttons::Quit)
            result = Selection::Quit;
	}
	else if (buffer == "0")
	{
		if (buttons == Buttons::YesNo)
			result = Selection::No;
		else if (buttons == Buttons::OKCancel)
			result = Selection::Cancel;
		else if (buttons == Buttons::OK)
			result = Selection::Quit;
        else if (buttons == Buttons::Quit)
            result = Selection::Quit;
	}
	else
		result = Selection::Quit;

	return result;
}

#endif