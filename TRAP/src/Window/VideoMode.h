#ifndef _TRAP_VIDEOMODE_H_
#define _TRAP_VIDEOMODE_H_

namespace TRAP
{
	//This describes a single video mode.
	struct VideoMode
	{
		//The width, in screen coordinates, of the video mode.
		int32_t Width = 0;
		//The height, in screen coordinates, of the video mode.
		int32_t Height = 0;
		//The bit depth of the red channel of the video mode.
		int32_t RedBits = 0;
		//The bit depth of the green channel of the video mode.
		int32_t GreenBits = 0;
		//The bit depth of the blue channel of the video mode.
		int32_t BlueBits = 0;
		//The refresh rate, in Hz, of the video mode.
		int32_t RefreshRate = 0;
	};
}

#endif /*_TRAP_VIDEOMODE_H_*/