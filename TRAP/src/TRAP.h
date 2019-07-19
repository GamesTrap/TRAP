#ifndef _TRAP_TRAP_H_
#define _TRAP_TRAP_H_

//For use by TRAP Applications
#include "Application.h"
#include "Layers/Layer.h"
#include "Log/Log.h"

//----INPUT-------------
#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Input/MouseButtonCodes.h"
//----------------------

//----EVENTS------------
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
//----------------------

#include "Layers/ImGuiLayer.h"

#include "VFS/VFS.h"

//----RENDERER----------
#include "Graphics/Renderer.h"
#include "Graphics/RenderCommand.h"

#include "Graphics/Buffers/BufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"

#include "Graphics/Shaders/Shader.h"

#include "Graphics/OrthographicCamera.h"
//----------------------

//----ENTRY POINT-------
#include "EntryPoint.h"
//----------------------

#endif /*_TRAP_TRAP_H_*/