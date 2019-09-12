#ifndef _TRAP_TRAP_H_
#define _TRAP_TRAP_H_

//For use by TRAP Applications
#include "Application.h"
#include "Layers/Layer.h"
#include "Log/Log.h"

//----INPUT-------------
#include "Input/Input.h"
//----------------------

//----EVENTS------------
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
//----------------------

#include "Layers/ImGuiLayer.h"

#include "VFS/VFS.h"

//----UTILS-------------
#include "Utils/MsgBox/MsgBox.h"
#include "Utils/TimeStep.h"
#include "Utils/Timer.h"
#include "Utils/Singleton.h"
#include "Utils/String.h"
//----------------------

//----MATHS-------------
#include "Maths/Maths.h"
//----------------------

//----RENDERER----------
#include "Graphics/Renderer.h"
#include "Graphics/RenderCommand.h"

#include "Graphics/Buffers/BufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/UniformBuffer.h"
#include "Graphics/Buffers/FrameBuffers/FrameBuffer.h"
#include "Graphics/Buffers/FrameBuffers/FrameBuffer2D.h"

#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Shaders/Shader.h"

#include "Graphics/Textures/TextureManager.h"
#include "ImageLoader/Image.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"

#include "Graphics/Cameras/Camera.h"
#include "Graphics/Cameras/OrthographicCamera.h"
//----------------------

//----ENTRY POINT-------
#include "EntryPoint.h"
//----------------------


#endif /*_TRAP_TRAP_H_*/