#ifndef _TRAP_TRAPPCH_H_
#define _TRAP_TRAPPCH_H_

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <set>
#include <filesystem>
#include <optional>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <thread>

#include <vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef TRAP_PLATFORM_MACOSX
#include <Cocoa/Cocoa.h>
#include <AvailabilityMacros.h>
#endif

#ifdef TRAP_PLATFORM_LINUX
#include <gtk/gtk.h>
#endif

#ifdef TRAP_PLATFORM_WINDOWS
#include <dxgi1_6.h>
#include <d3d12.h>
#include "Utils/Win.h"
#endif

#include "Utils/MsgBox/MsgBox.h"

#include "Utils/Logo.h"

#include "Utils/Timer.h"
#include "Utils/TimeStep.h"

#include "Core.h"
#include "Log/Log.h"
#include "Utils/String.h"
#include "Utils/NonMovable.h"
#include "Utils/NonCopyable.h"
#include "Utils/Singleton.h"
#include "Utils/Exception.h"

#include "VFS/FileWatcher.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

#include "Config/Config.h"

#include "Maths/Maths.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

#include "Graphics/Shaders/ShaderUniform.h"
#include "Graphics/Shaders/ShaderResource.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderFactory.h"
#include "Graphics/Shaders/ShaderManager.h"

#include "Graphics/API/Vulkan/VulkanShader.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/D3D12Shader.h"
#endif
#include "Graphics/API/OpenGL/OpenGLShaderUniform.h"
#include "Graphics/API/OpenGL/OpenGLShaderResource.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/D3D12Common.h"
#endif

#include "Graphics/API/Renderer.h"

#include "Window/Window.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/OpenGL/OpenGLContext.h"
#include "Graphics/API/Vulkan/VulkanContext.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/D3D12Context.h"
#endif

#include "Graphics/API/OpenGL/OpenGLRenderer.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#ifdef TRAP_PLATFORM_WINDOWS
#include "Graphics/API/D3D12/D3D12Renderer.h"
#endif

#include "Layers/Layer.h"
#include "Layers/LayerStack.h"

#include "Application.h"

#include "imgui.h"

#endif /*_TRAP_TRAPPCH_H_*/