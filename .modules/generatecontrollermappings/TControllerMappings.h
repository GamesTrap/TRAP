#ifndef TRAP_CONTROLLERMAPPINGS_H
#define TRAP_CONTROLLERMAPPINGS_H

#ifndef TRAP_HEADLESS_MODE

#include <array>
#include <string_view>

#include "Core/Types.h"

/*
Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
//Updates can be found at: https://github.com/gabomdq/SDL_GameControllerDB
//Modified by Jan "GamesTrap" Schuerkamp 2023-2024

namespace TRAP::Embed
{
#ifdef TRAP_BUILD_WIN32_MAPPINGS
	constexpr usize ControllerMappingsSize = @TRAP_WIN32_MAPPINGS_SIZE@;
#endif /*TRAP_BUILD_WIN32_MAPPINGS*/

#ifdef TRAP_BUILD_LINUX_MAPPINGS
	constexpr usize ControllerMappingsSize = @TRAP_LINUX_MAPPINGS_SIZE@;
#endif /*TRAP_BUILD_LINUX_MAPPINGS*/

	constexpr std::array<std::string_view, ControllerMappingsSize> ControllerMappings =
	{
#ifdef TRAP_BUILD_WIN32_MAPPINGS
@TRAP_WIN32_MAPPINGS@
		"78696e70757401000000000000000000,XInput Gamepad,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757402000000000000000000,XInput Wheel,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757403000000000000000000,XInput Arcade Stick,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757404000000000000000000,XInput Flight Stick,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757405000000000000000000,XInput Dance Pad,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757406000000000000000000,XInput Guitar,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
		"78696e70757408000000000000000000,XInput Drum Kit,platform:Windows,a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,back:b6,start:b7,leftstick:b8,rightstick:b9,guide:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,",
#endif /*TRAP_BUILD_WIN32_MAPPINGS*/

#ifdef TRAP_BUILD_LINUX_MAPPINGS
@TRAP_LINUX_MAPPINGS@
#endif /*TRAP_BUILD_LINUX_MAPPINGS*/
	};
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_CONTROLLERMAPPINGS_H*/