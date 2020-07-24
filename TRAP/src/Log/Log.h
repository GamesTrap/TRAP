#ifndef _TRAP_LOG_H_
#define _TRAP_LOG_H_

#ifdef TRAP_PLATFORM_WINDOWS
	#include "Utils/Win.h"
#endif

namespace TRAP
{
	class Log final
	{
	private:
		Log();
	public:
		~Log();
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
		Log(Log&&) = delete;
		Log& operator=(Log&&) = delete;
		
		enum class Level
		{
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Critical
		};

		static Log& Get();

		template<typename... Args>
		static void Trace(Args&& ... args);

		template<typename... Args>
		static void Debug(Args&& ... args);

		template<typename... Args>
		static void Info(Args&& ... args);

		template<typename... Args>
		static void Warn(Args&& ... args);

		template<typename... Args>
		static void Error(Args&& ... args);

		template<typename... Args>
		static void Critical(Args&& ... args);

		static const std::vector<std::pair<Level, std::string>>& GetBuffer();

		static void Save();
		static void Clear();

		static constexpr auto WindowVersion = "[20w30a3]";
		static constexpr auto WindowPrefix = "[Window] ";
		static constexpr auto WindowInternalPrefix = "[Window][Internal] ";
		static constexpr auto WindowIconPrefix = "[Window][Icon] ";
		static constexpr auto ConfigPrefix = "[Config] ";
		static constexpr auto ApplicationPrefix = "[Application] ";
		static constexpr auto ShaderManagerPrefix = "[ShaderManager] ";
		static constexpr auto ShaderPrefix = "[Shader] ";
		static constexpr auto ShaderSPIRVPrefix = "[Shader][SPIR-V] ";
		static constexpr auto ShaderVulkanPrefix = "[Shader][Vulkan] ";
		static constexpr auto ShaderVulkanSPIRVPrefix = "[Shader][Vulkan][SPIR-V] ";
		static constexpr auto ShaderVulkanGLSLPrefix = "[Shader][Vulkan][GLSL] ";
		static constexpr auto ShaderOpenGLPrefix = "[Shader][OpenGL] ";
		static constexpr auto ShaderOpenGLGLSLPrefix = "[Shader][OpenGL][GLSL] ";
		static constexpr auto ShaderOpenGLGLSLProgramPrefix = "[Shader][OpenGL][GLSL][Program] ";
		static constexpr auto ShaderOpenGLSPIRVPrefix = "[Shader][OpenGL][SPIR-V] ";
		static constexpr auto TextureManagerPrefix = "[TextureManager] ";
		static constexpr auto TextureManagerTextureCubePrefix = "[TextureManager][TextureCube] ";
		static constexpr auto TextureManagerTexture2DPrefix = "[TextureManager][Texture2D] ";
		static constexpr auto TextureCubePrefix = "[TextureCube] ";
		static constexpr auto TextureCubeVulkanPrefix = "[TextureCube][Vulkan] ";
		static constexpr auto TextureCubeOpenGLPrefix = "[TextureCube][OpenGL] ";
		static constexpr auto Texture2DPrefix = "[Texture2D] ";
		static constexpr auto Texture2DVulkanPrefix = "[Texture2D][Vulkan] ";
		static constexpr auto Texture2DOpenGLPrefix = "[Texture2D][OpenGL] ";
		static constexpr auto Texture2DOpenGLPerformancePrefix = "[Texture2D][OpenGL][Performance] ";
		static constexpr auto EnginePrefix = "[Engine] ";
		static constexpr auto EngineLinuxPrefix = "[Engine][Linux] ";
		static constexpr auto EngineLinuxX11Prefix = "[Engine][Linux][X11] ";
		static constexpr auto EngineLinuxWaylandPrefix = "[Engine][Linux][Wayland] ";
		static constexpr auto EngineWindowsPrefix = "[Engine][Windows] ";
		static constexpr auto VFSPrefix = "[VFS] ";
		static constexpr auto FileSystemPrefix = "[FileSystem] ";
		static constexpr auto InstrumentorPrefix = "[Instrumentor] ";
		static constexpr auto MsgBoxLinuxX11Prefix = "[MsgBox][Linux][X11] ";
		static constexpr auto LoggerPrefix = "[Logger] ";
		static constexpr auto LayerStackPrefix = "[LayerStack] ";
		static constexpr auto ImGuiOpenGLPrefix = "[ImGui][OpenGL] ";
		static constexpr auto InputControllerDirectInputPrefix = "[Input][Controller][Windows][DirectInput] ";
		static constexpr auto InputControllerXInputPrefix = "[Input][Controller][Windows][XInput] ";
		static constexpr auto InputControllerLinuxPrefix = "[Input][Controller][Linux] ";
		static constexpr auto InputControllerPrefix = "[Input][Controller] ";
		static constexpr auto InputWinAPIPrefix = "[Input][Windows][WinAPI] ";
		static constexpr auto InputPrefix = "[Input] ";
		static constexpr auto ImagePrefix = "[Image] ";
		static constexpr auto ImageTGAPrefix = "[Image][TGA] ";
		static constexpr auto ImagePNGPrefix = "[Image][PNG] ";
		static constexpr auto ImagePPMPrefix = "[Image][PPM] ";
		static constexpr auto ImagePNMPrefix = "[Image][PNM] ";
		static constexpr auto ImagePGMPrefix = "[Image][PGM] ";
		static constexpr auto ImagePFMPrefix = "[Image][PFM] ";
		static constexpr auto ImagePAMPrefix = "[Image][PAM] ";
		static constexpr auto ImageBMPPrefix = "[Image][BMP] ";
		static constexpr auto RendererPrefix = "[Renderer] ";
		static constexpr auto RendererVulkanPrefix = "[Renderer][Vulkan] ";
		static constexpr auto RendererVulkanPhysicalDevicePrefix = "[Renderer][Vulkan][PhysicalDevice] ";
		static constexpr auto RendererVulkanInstancePrefix = "[Renderer][Vulkan][Instance] ";
		static constexpr auto RendererOpenGLPrefix = "[Renderer][OpenGL] ";
		static constexpr auto Renderer2DPrefix = "[Renderer2D] ";
		static constexpr auto ContextPrefix = "[Context] ";
		static constexpr auto ContextOpenGLPrefix = "[Context][OpenGL] ";
		static constexpr auto ContextVulkanPrefix = "[Context][Vulkan] ";
		static constexpr auto UniformBufferVulkanPrefix = "[UniformBuffer][Vulkan] ";
		static constexpr auto UniformBufferOpenGLPrefix = "[UniformBuffer][OpenGL] ";
		static constexpr auto FrameBufferVulkanPrefix = "[FrameBuffer][Vulkan] ";
		static constexpr auto FrameBufferOpenGLPrefix = "[FrameBuffer][OpenGL] ";
		
#ifdef TRAP_PLATFORM_WINDOWS
	private:
		static void GetInfo() { GetConsoleScreenBufferInfo(m_handleConsole, &m_csbi); }
		static void SetColor(WORD wRGBI);
		static void ResetColor() { SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }

		static HANDLE m_handleConsole;
		static CONSOLE_SCREEN_BUFFER_INFO m_csbi;
#else
	private:
		static bool IsColorTerminal() noexcept;
#endif
		static std::string GetTimeStamp();

		std::vector<std::pair<Level, std::string>> m_buffer{};

		static std::mutex m_mtx;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#include "Log.inl"

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_TRACE(const Args& ... args)
{
	::TRAP::Log::Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
	::TRAP::Log::Debug(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#else
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_INFO(const Args& ... args)
{
	::TRAP::Log::Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_WARN(const Args& ... args)
{
	::TRAP::Log::Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_ERROR(const Args& ... args)
{
	::TRAP::Log::Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_CRITICAL(const Args& ... args)
{
	::TRAP::Log::Critical(args...);
}

#endif /*_TRAP_LOG_H_*/