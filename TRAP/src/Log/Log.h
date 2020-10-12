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
		/// <summary>
		/// Private Constructor.
		/// </summary>
		Log();
	public:
		/// <summary>
		///	Destructor.
		/// </summary>
		~Log();
		/// <summary>
		/// Deleted Copy Constructor.
		/// </summary>
		Log(const Log&) = delete;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		Log& operator=(const Log&) = delete;
		/// <summary>
		/// Deleted Move Constructor.
		/// </summary>
		Log(Log&&) = delete;
		/// <summary>
		/// Deleted Move Assignment Operator.
		/// </summary>
		Log& operator=(Log&&) = delete;

		/// <summary>
		/// Importance of a log message.
		/// </summary>
		enum class Level
		{
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Critical
		};

		/// <summary>
		/// Get the singleton Log instance.
		/// </summary>
		/// <returns>Singleton Log instance.</returns>
		static Log& Get();

		/// <summary>
		/// Log a Trace message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Trace(Args&& ... args);

		/// <summary>
		/// Log a Debug message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Debug(Args&& ... args);

		/// <summary>
		/// Log a Info message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Info(Args&& ... args);

		/// <summary>
		/// Log a Warn message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Warn(Args&& ... args);

		/// <summary>
		/// Log a Error message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Error(Args&& ... args);

		/// <summary>
		/// Log a Critical message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		static void Critical(Args&& ... args);

		/// <summary>
		/// Get all saves log messages and their importance level.
		/// </summary>
		/// <returns>Messages and importance level in a vector.</returns>
		static const std::vector<std::pair<Level, std::string>>& GetBuffer();

		/// <summary>
		/// Save all collected messages into "TRAP.log" file.
		/// </summary>
		static void Save();
		/// <summary>
		/// Clears (and also Saves) all collected messages.
		/// </summary>
		static void Clear();

		inline static constexpr auto WindowVersion = "[20w42a1]";
		inline static constexpr auto WindowPrefix = "[Window] ";
		inline static constexpr auto WindowInternalPrefix = "[Window][Internal] ";
		inline static constexpr auto WindowIconPrefix = "[Window][Icon] ";
		inline static constexpr auto ConfigPrefix = "[Config] ";
		inline static constexpr auto ApplicationPrefix = "[Application] ";
		inline static constexpr auto ShaderManagerPrefix = "[ShaderManager] ";
		inline static constexpr auto ShaderPrefix = "[Shader] ";
		inline static constexpr auto ShaderSPIRVPrefix = "[Shader][SPIR-V] ";
		inline static constexpr auto ShaderVulkanPrefix = "[Shader][Vulkan] ";
		inline static constexpr auto ShaderVulkanSPIRVPrefix = "[Shader][Vulkan][SPIR-V] ";
		inline static constexpr auto ShaderVulkanGLSLPrefix = "[Shader][Vulkan][GLSL] ";
		inline static constexpr auto ShaderOpenGLPrefix = "[Shader][OpenGL] ";
		inline static constexpr auto ShaderOpenGLGLSLPrefix = "[Shader][OpenGL][GLSL] ";
		inline static constexpr auto ShaderOpenGLGLSLProgramPrefix = "[Shader][OpenGL][GLSL][Program] ";
		inline static constexpr auto ShaderOpenGLSPIRVPrefix = "[Shader][OpenGL][SPIR-V] ";
		inline static constexpr auto TextureManagerPrefix = "[TextureManager] ";
		inline static constexpr auto TextureManagerTextureCubePrefix = "[TextureManager][TextureCube] ";
		inline static constexpr auto TextureManagerTexture2DPrefix = "[TextureManager][Texture2D] ";
		inline static constexpr auto TextureCubePrefix = "[TextureCube] ";
		inline static constexpr auto TextureCubeVulkanPrefix = "[TextureCube][Vulkan] ";
		inline static constexpr auto TextureCubeOpenGLPrefix = "[TextureCube][OpenGL] ";
		inline static constexpr auto Texture2DPrefix = "[Texture2D] ";
		inline static constexpr auto Texture2DVulkanPrefix = "[Texture2D][Vulkan] ";
		inline static constexpr auto Texture2DOpenGLPrefix = "[Texture2D][OpenGL] ";
		inline static constexpr auto Texture2DOpenGLPerformancePrefix = "[Texture2D][OpenGL][Performance] ";
		inline static constexpr auto EnginePrefix = "[Engine] ";
		inline static constexpr auto EngineLinuxPrefix = "[Engine][Linux] ";
		inline static constexpr auto EngineLinuxX11Prefix = "[Engine][Linux][X11] ";
		inline static constexpr auto EngineLinuxWaylandPrefix = "[Engine][Linux][Wayland] ";
		inline static constexpr auto EngineWindowsPrefix = "[Engine][Windows] ";
		inline static constexpr auto VFSPrefix = "[VFS] ";
		inline static constexpr auto FileSystemPrefix = "[FileSystem] ";
		inline static constexpr auto InstrumentorPrefix = "[Instrumentor] ";
		inline static constexpr auto MsgBoxLinuxX11Prefix = "[MsgBox][Linux][X11] ";
		inline static constexpr auto LoggerPrefix = "[Logger] ";
		inline static constexpr auto LayerStackPrefix = "[LayerStack] ";
		inline static constexpr auto ImGuiOpenGLPrefix = "[ImGui][OpenGL] ";
		inline static constexpr auto InputControllerDirectInputPrefix = "[Input][Controller][Windows][DirectInput] ";
		inline static constexpr auto InputControllerXInputPrefix = "[Input][Controller][Windows][XInput] ";
		inline static constexpr auto InputControllerLinuxPrefix = "[Input][Controller][Linux] ";
		inline static constexpr auto InputControllerPrefix = "[Input][Controller] ";
		inline static constexpr auto InputWinAPIPrefix = "[Input][Windows][WinAPI] ";
		inline static constexpr auto InputPrefix = "[Input] ";
		inline static constexpr auto ImagePrefix = "[Image] ";
		inline static constexpr auto ImageTGAPrefix = "[Image][TGA] ";
		inline static constexpr auto ImagePNGPrefix = "[Image][PNG] ";
		inline static constexpr auto ImagePPMPrefix = "[Image][PPM] ";
		inline static constexpr auto ImagePNMPrefix = "[Image][PNM] ";
		inline static constexpr auto ImagePGMPrefix = "[Image][PGM] ";
		inline static constexpr auto ImagePFMPrefix = "[Image][PFM] ";
		inline static constexpr auto ImagePAMPrefix = "[Image][PAM] ";
		inline static constexpr auto ImageBMPPrefix = "[Image][BMP] ";
		inline static constexpr auto ImageRadiancePrefix = "[Image][Radiance] ";
		inline static constexpr auto RendererPrefix = "[Renderer] ";
		inline static constexpr auto RendererVulkanPrefix = "[Renderer][Vulkan] ";
		inline static constexpr auto RendererVulkanPhysicalDevicePrefix = "[Renderer][Vulkan][PhysicalDevice] ";
		inline static constexpr auto RendererVulkanInstancePrefix = "[Renderer][Vulkan][Instance] ";
		inline static constexpr auto RendererOpenGLPrefix = "[Renderer][OpenGL] ";
		inline static constexpr auto Renderer2DPrefix = "[Renderer2D] ";
		inline static constexpr auto ContextPrefix = "[Context] ";
		inline static constexpr auto ContextOpenGLPrefix = "[Context][OpenGL] ";
		inline static constexpr auto ContextVulkanPrefix = "[Context][Vulkan] ";
		inline static constexpr auto UniformBufferVulkanPrefix = "[UniformBuffer][Vulkan] ";
		inline static constexpr auto UniformBufferOpenGLPrefix = "[UniformBuffer][OpenGL] ";
		inline static constexpr auto FrameBufferVulkanPrefix = "[FrameBuffer][Vulkan] ";
		inline static constexpr auto FrameBufferOpenGLPrefix = "[FrameBuffer][OpenGL] ";
		inline static constexpr auto NetworkSocketPrefix = "[Network][Socket] ";
		inline static constexpr auto NetworkFTPPrefix = "[Network][FTP] ";
		inline static constexpr auto NetworkHTTPPrefix = "[Network][HTTP] ";
		inline static constexpr auto NetworkTCPListenerPrefix = "[Network][TCPListener] ";
		inline static constexpr auto NetworkTCPSocketPrefix = "[Network][TCPSocket] ";
		inline static constexpr auto NetworkUDPSocketPrefix = "[Network][UDPSocket] ";
		inline static constexpr auto NetworkSocketUnixPrefix = "[Network][Socket][Unix] ";
		
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
		/// <summary>
		/// Get a TimeStamp with [HH:MM:SS] format.
		/// </summary>
		/// <returns>TimeStamp as a string.</returns>
		static std::string GetTimeStamp();

		std::vector<std::pair<Level, std::string>> m_buffer{};

		static std::mutex m_mtx;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#include "Log.inl"

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a Trace message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_TRACE(const Args& ... args)
{
	::TRAP::Log::Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)

/// <summary>
/// Log a Debug message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
	::TRAP::Log::Debug(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#else
/// <summary>
/// Log a Debug message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a Info message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_INFO(const Args& ... args)
{
	::TRAP::Log::Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a Warn message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_WARN(const Args& ... args)
{
	::TRAP::Log::Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a Error message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_ERROR(const Args& ... args)
{
	::TRAP::Log::Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a Critical message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
constexpr void TP_CRITICAL(const Args& ... args)
{
	::TRAP::Log::Critical(args...);
}

#endif /*_TRAP_LOG_H_*/