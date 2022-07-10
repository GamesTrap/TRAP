#ifndef TRAP_LINUX_H
#define TRAP_LINUX_H

#include <linux/input.h>
#include <linux/limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/shape.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/Xmd.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pwd.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <netdb.h>

#ifdef None
	#undef None
#endif
#ifdef Bool
	#undef Bool
#endif
#ifdef Always
	#undef Always
#endif
#ifdef True
	#undef True
#endif
#ifdef False
	#undef False
#endif
#ifdef KeyPress
	#undef KeyPress
#endif
#ifdef KeyRelease
	#undef KeyRelease
#endif
#ifdef GrayScale
	#undef GrayScale
#endif
#ifdef Status
	#undef Status
#endif
#ifdef BadRequest
	#undef BadRequest
#endif
#ifdef Success
	#undef Success
#endif

#endif /*TRAP_LINUX_H*/