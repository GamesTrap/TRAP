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
#include "Event/Event.h"
#include "Event/WindowEvent.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/ControllerEvent.h"
//----------------------

#include "Layers/ImGui/ImGuiLayer.h"

#include "VFS/VFS.h"

//----UTILS-------------
#include "Utils/MsgBox/MsgBox.h"
#include "Utils/TimeStep.h"
#include "Utils/Timer.h"
#include "Utils/Singleton.h"
#include "Utils/String.h"
//----------------------

//----MATHS-------------
#include "Maths/Math.h"
#include "Maths/Vec2.h"
#include "Maths/Vec3.h"
#include "Maths/Vec4.h"
#include "Maths/Mat3.h"
#include "Maths/Mat4.h"
#include "Maths/Types.h"
//----------------------

//----WINDOWS-----------
#include "Window/Window.h"
//----------------------

//----MONITORS----------
#include "Window/Monitor.h"
//----------------------

//----RENDERER----------
#include "Graphics/Renderer.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Renderer2D.h"

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
#include "Graphics/Cameras/Orthographic/OrthographicCamera.h"
#include "Graphics/Cameras/Orthographic/OrthographicCameraController.h"
//----------------------

#endif /*_TRAP_TRAP_H_*/