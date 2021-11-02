#ifndef TRAP_TRAP_H
#define TRAP_TRAP_H

//----BASE---------------
#include "../src/Core/Base.h"
//-----------------------

//----ASSERT-------------
#include "../src/TRAP_Assert.h"
//-----------------------

//----APPLICATION--------
#include "../src/Application.h"
//-----------------------

//----LOG----------------
#include "../src/Log/Log.h"
//-----------------------

//----INPUT--------------
#include "../src/Input/Input.h"
//-----------------------

//----EVENTS-------------
#include "../src/Events/Event.h"
#include "../src/Events/WindowEvent.h"
#include "../src/Events/ApplicationEvent.h"
#include "../src/Events/KeyEvent.h"
#include "../src/Events/MouseEvent.h"
#include "../src/Events/ControllerEvent.h"
//-----------------------

//----LAYERS-------------
#include "../src/Layers/Layer.h"
#include "../src/Layers/ImGui/ImGuiLayer.h"
//-----------------------

//----VIRTUAL-FILE-SYSTEM
#include "../src/VFS/VFS.h"
//-----------------------

//----UTILS--------------
#include "../src/Utils/Utils.h"
#include "../src/Utils/Dialogs/Dialogs.h"
#include "../src/Utils/Time/TimeStep.h"
#include "../src/Utils/Time/Timer.h"
#include "../src/Utils/String/String.h"
#include "../src/Utils/Random/Random.h"
#include "../src/Utils/Hash/Adler32.h"
#include "../src/Utils/Hash/CRC32.h"
#include "../src/Utils/Hash/SHA-2.h"
#include "../src/Utils/Hash/SHA-3.h"
#include "../src/Utils/Hash/ConvertHashToString.h"
#include "../src/Utils/Discord/DiscordGameSDK.h"
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
#include "../src/Graphics/Buffers/VertexBufferLayout.h"
#include "../src/Graphics/Buffers/IndexBuffer.h"
#include "../src/Graphics/Buffers/VertexBuffer.h"
#include "../src/Graphics/Buffers/UniformBuffer.h"
#include "../src/Graphics/Buffers/StorageBuffer.h"
#include "../src/Graphics/Buffers/FrameBuffer.h"
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
#include "../src/Graphics/API/Objects/Sampler.h"
//-----------------------

//----CAMERAS------------
#include "../src/Graphics/Cameras/Camera.h"
#include "../src/Graphics/Cameras/Orthographic/OrthographicCamera.h"
#include "../src/Graphics/Cameras/Orthographic/OrthographicCameraController.h"
//-----------------------

//----NETWORK------------
#include "../src/Network/Network.h"
//-----------------------

//----SCENE & COMPONENTS-
#include "../src/Scene/Scene.h"
#include "../src/Scene/Components.h"
#include "../src/Scene/Entity.h"
#include "../src/Scene/ScriptableEntity.h"
//-----------------------

#endif /*TRAP_TRAP_H*/