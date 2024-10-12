#ifndef TRAP_TRAP_H
#define TRAP_TRAP_H

//----BASE---------------
#include "../src/Core/Base.h"
#include "../src/Core/Types.h"
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

#ifndef TRAP_HEADLESS_MODE
//----INPUT--------------
#include "../src/Input/Input.h"
//-----------------------
#endif /*TRAP_HEADLESS_MODE*/

//----EVENTS-------------
#include "../src/Events/Event.h"
#include "../src/Events/HotReloadEvent.h"
#include "../src/Events/FileEvent.h"
#ifndef TRAP_HEADLESS_MODE
#include "../src/Events/WindowEvent.h"
#include "../src/Events/MonitorEvent.h"
#include "../src/Events/KeyEvent.h"
#include "../src/Events/MouseEvent.h"
#include "../src/Events/ControllerEvent.h"
#endif /*TRAP_HEADLESS_MODE*/
//-----------------------

//----LAYERS-------------
#include "../src/Layers/Layer.h"
#ifndef TRAP_HEADLESS_MODE
#include "../src/Layers/ImGui/ImGuiLayer.h"
#endif /*TRAP_HEADLESS_MODE*/
//-----------------------

//----FILE-SYSTEM--------
#include "../src/FileSystem/FileSystem.h"
#include "../src/FileSystem/FileSystemWatcher.h"
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
#include "../src/Utils/Hash/UID.h"
#include "../src/Utils/Hash/ConvertHashToString.h"
#include "../src/Utils/NumericCasts.h"
#ifndef TRAP_HEADLESS_MODE
#include "../src/Utils/Discord/DiscordGameSDK.h"
#endif /*TRAP_HEADLESS_MODE*/
#include "../src/Utils/Steam/SteamworksSDK.h"
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

#ifndef TRAP_HEADLESS_MODE
//----WINDOWS------------
#include "../src/Window/Window.h"
//-----------------------
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
//----MONITORS-----------
#include "../src/Window/Monitor.h"
//-----------------------
#endif /*TRAP_HEADLESS_MODE*/

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
#include "../src/Graphics/API/Objects/RenderTarget.h"
//-----------------------

//----SHADERS------------
#include "../src/Graphics/Shaders/ShaderManager.h"
#include "../src/Graphics/Shaders/Shader.h"
//-----------------------

//----TEXTURES-----------
#include "../src/Graphics/Textures/TextureManager.h"
#include "../src/Graphics/Textures/SpriteManager.h"
#include "../src/ImageLoader/Image.h"
#include "../src/Graphics/Textures/Texture.h"
#include "../src/Graphics/Textures/SubTexture2D.h"
#include "../src/Graphics/Textures/SpriteAnimation.h"
#include "../src/Graphics/API/Objects/Sampler.h"
//-----------------------

//----CAMERAS------------
#include "../src/Graphics/Cameras/Camera.h"
#include "../src/Graphics/Cameras/Orthographic/OrthographicCamera.h"
#ifndef TRAP_HEADLESS_MODE
#include "../src/Graphics/Cameras/Orthographic/OrthographicCameraController.h"
#include "../src/Graphics/Cameras/Editor/EditorCamera.h"
#endif /*TRAP_HEADLESS_MODE*/
//-----------------------

//----NETWORK------------
#include "../src/Network/Network.h"
//-----------------------

//----SCENE & COMPONENTS-
#include "../src/Scene/SceneSerializer.h"
#include "../src/Scene/Scene.h"
#include "../src/Scene/Components.h"
#include "../src/Scene/Entity.h"
#include "../src/Scene/ScriptableEntity.h"
//-----------------------

#endif /*TRAP_TRAP_H*/
