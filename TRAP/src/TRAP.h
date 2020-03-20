#ifndef _TRAP_TRAP_H_
#define _TRAP_TRAP_H_

//----APPLICATION--------
//#include "Application.h"
#include "../src/Application.h"
//-----------------------

//----LOG----------------
#include "../src/Log/Log.h"
//-----------------------

//----INPUT--------------
#include "../src/Input/Input.h"
//-----------------------

//----EVENTS-------------
#include "../src/Event/Event.h"
#include "../src/Event/WindowEvent.h"
#include "../src/Event/ApplicationEvent.h"
#include "../src/Event/KeyEvent.h"
#include "../src/Event/MouseEvent.h"
#include "../src/Event/ControllerEvent.h"
//-----------------------

//----LAYERS-------------
#include "../src/Layers/Layer.h"
#include "../src/Layers/ImGui/ImGuiLayer.h"
//-----------------------

//----VIRTUAL-FILE-SYSTEM
#include "../src/VFS/VFS.h"
//-----------------------

//----UTILS--------------
#include "../src/Utils/MsgBox/MsgBox.h"
#include "../src/Utils/Time/TimeStep.h"
#include "../src/Utils/Time/Timer.h"
#include "../src/Utils/String/String.h"
#include "../src/Utils/Random/Random.h"
#include "../src/Utils/Hash/Adler32.h"
#include "../src/Utils/Hash/CRC32.h"
#include "../src/Utils/Hash/SHA-2.h"
#include "../src/Utils/Hash/SHA-3.h"
#include "../src/Utils/Hash/ConvertHashToString.h"
//-----------------------

//----DEBUG--------------
#include "../src/Utils/Profiler/Instrumentor.h"
//-----------------------

//----MATHS--------------
#include "../src/Maths/Math.h"
#include "../src/Maths/Vec2.h"
#include "../src/Maths/Vec3.h"
#include "../src/Maths/Vec4.h"
#include "../src/Maths/Mat3.h"
#include "../src/Maths/Mat4.h"
#include "../src/Maths/Quaternion.h"
#include "../src/Maths/Types.h"
//-----------------------

//----WINDOWS------------
#include "../src/Window/Window.h"
//-----------------------

//----MONITORS-----------
#include "../src/Window/Monitor.h"
//-----------------------

//----RENDERER-----------
#include "../src/Graphics/Renderer.h"
#include "../src/Graphics/RenderCommand.h"
#include "../src/Graphics/Renderer2D.h"
//-----------------------

//----BUFFERS------------
#include "../src/Graphics/Buffers/BufferLayout.h"
#include "../src/Graphics/Buffers/IndexBuffer.h"
#include "../src/Graphics/Buffers/VertexArray.h"
#include "../src/Graphics/Buffers/VertexBuffer.h"
#include "../src/Graphics/Buffers/UniformBuffer.h"
#include "../src/Graphics/Buffers/FrameBuffers/FrameBuffer.h"
#include "../src/Graphics/Buffers/FrameBuffers/FrameBuffer2D.h"
//-----------------------

//----SHADERS------------
#include "../src/Graphics/Shaders/ShaderManager.h"
#include "../src/Graphics/Shaders/Shader.h"
//-----------------------

//----TEXTURES-----------
#include "../src/Graphics/Textures/TextureManager.h"
#include "../src/ImageLoader/Image.h"
#include "../src/Graphics/Textures/Texture.h"
#include "../src/Graphics/Textures/Texture2D.h"
#include "../src/Graphics/Textures/TextureCube.h"
//-----------------------

//----CAMERAS------------
#include "../src/Graphics/Cameras/Camera.h"
#include "../src/Graphics/Cameras/Orthographic/OrthographicCamera.h"
#include "../src/Graphics/Cameras/Orthographic/OrthographicCameraController.h"
//-----------------------

#endif /*_TRAP_TRAP_H_*/