#ifndef TRAP_LINUX_H
#define TRAP_LINUX_H

#include "Core/PlatformDetection.h"

#ifdef TRAP_PLATFORM_LINUX

#include <cpuid.h>

#include <linux/input.h>
#include <linux/limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <csignal>
#include <pwd.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <netdb.h>

//Wayland
#include <wayland-client-core.h>
#include "wayland-client-protocol.h"
#include "wayland-xdg-shell-client-protocol.h"
#include "wayland-xdg-decoration-client-protocol.h"
#include "wayland-viewporter-client-protocol.h"
#include "wayland-relative-pointer-unstable-v1-client-protocol.h"
#include "wayland-pointer-constraints-unstable-v1-client-protocol.h"
#include "wayland-idle-inhibit-unstable-v1-client-protocol.h"
#include "wayland-xdg-activation-v1-client-protocol.h"
#include "wayland-content-type-v1-client-protocol.h"
#include "wayland-fractional-scale-v1-client-protocol.h"
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

//X11
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

#ifdef None
	#undef None
#endif /*None*/
#ifdef Bool
	#undef Bool
#endif /*Bool*/
#ifdef Always
	#undef Always
#endif /*Always*/
#ifdef True
	#undef True
#endif /*True*/
#ifdef False
	#undef False
#endif /*False*/
#ifdef KeyPress
	#undef KeyPress
#endif /*KeyPress*/
#ifdef KeyRelease
	#undef KeyRelease
#endif /*KeyRelease*/
#ifdef GrayScale
	#undef GrayScale
#endif /*GrayScale*/
#ifdef Status
	#undef Status
#endif /*Status*/
#ifdef BadRequest
	#undef BadRequest
#endif /*BadRequest*/
#ifdef Success
	#undef Success
#endif /*Success*/
#ifdef XNegative
    #undef XNegative
#endif /*XNegative*/
#ifdef YNegative
    #undef YNegative
#endif /*YNegative*/

#endif /*TRAP_PLATFORM_LINUX*/

#endif /*TRAP_LINUX_H*/
