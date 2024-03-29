#ifndef TRAP_LOG_H
#define TRAP_LOG_H

#ifdef TRAP_PLATFORM_WINDOWS
	#include "Utils/Win.h"
#endif

#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>

#include "Maths/Types.h"

namespace TRAP
{
	/// <summary>
	/// Utility class to log messages to console.
	/// </summary>
	class Log final
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit Log();
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filePath">Path for the log file.</param>
		explicit Log(std::filesystem::path filePath);
		/// <summary>
		///	Destructor.
		/// </summary>
		~Log();
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Log(const Log&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Log& operator=(const Log&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Log(Log&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Log& operator=(Log&&) = delete;

		/// <summary>
		/// Get the current used file path for saving.
		/// </summary>
		/// <returns>File path.</returns>
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// <summary>
		/// Set the file path used for saving.
		///
		///	Logs files are always saved in the format: "<FileName>-YYYY-MM-DDTHH-MM-SS.<FileEnding>"
		/// </summary>
		/// <param name="filePath">File path.</param>
		void SetFilePath(std::filesystem::path filePath) noexcept;

		/// <summary>
		/// Importance levels.
		/// </summary>
		enum class Level : uint32_t
		{
			None = 0x00,
			Trace = 0x01,
			Debug = 0x02,
			Info = 0x04,
			Warn = 0x08,
			Error = 0x10,
			Critical = 0x20
		};

		/// <summary>
		/// Set the importance level for the log messages.
		/// Messages that are blow the given importance level
		/// won't be printed to the console.
		///
		/// Note: All messages are saved in the log file regardless
		///       off the importance level.
		/// </summary>
		/// <param name="level">Importance level to use.</param>
		constexpr void SetImportance(Level level) noexcept;

		/// <summary>
		/// Log a trace message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Trace(Args&& ... args);

		/// <summary>
		/// Log a debug message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Debug(Args&& ... args);

		/// <summary>
		/// Log a info message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Info(Args&& ... args);

		/// <summary>
		/// Log a warn message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Warn(Args&& ... args);

		/// <summary>
		/// Log a error message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Error(Args&& ... args);

		/// <summary>
		/// Log a critical message.
		/// </summary>
		/// <typeparam name="...Args">Message to log.</typeparam>
		template<typename... Args>
		void Critical(Args&& ... args);

		/// <summary>
		/// Get all saved log messages and their associated importance level.
		/// </summary>
		/// <returns>Messages with importance level.</returns>
		[[nodiscard]] constexpr const std::vector<std::pair<Level, std::string>>& GetBuffer() const noexcept;

		/// <summary>
		/// Save all collected messages to file.
		/// </summary>
		void Save() const;
		/// <summary>
		/// Clears all buffered messages.
		/// </summary>
		constexpr void Clear() noexcept;

		inline static constexpr auto WindowVersion =                        "[23w44a1]";
		inline static constexpr auto WindowPrefix =                         "[Window] ";
		inline static constexpr auto WindowIconPrefix =                     "[Window][Icon] ";
		inline static constexpr auto ConfigPrefix =                         "[Config] ";
		inline static constexpr auto ApplicationPrefix =                    "[Application] ";
		inline static constexpr auto ShaderManagerPrefix =                  "[ShaderManager] ";
		inline static constexpr auto ShaderPrefix =                         "[Shader] ";
		inline static constexpr auto ShaderGLSLPrefix =                     "[Shader][GLSL] ";
		inline static constexpr auto ShaderSPIRVPrefix =                    "[Shader][SPIR-V] ";
		inline static constexpr auto TextureManagerPrefix =                 "[TextureManager] ";
		inline static constexpr auto TextureManagerTexture2DPrefix =        "[TextureManager][Texture2D] ";
		inline static constexpr auto TextureManagerTextureCubePrefix =      "[TextureManager][TextureCube] ";
		inline static constexpr auto TexturePrefix =                        "[Texture] ";
		inline static constexpr auto SpriteManagerPrefix =                  "[SpriteManager] ";
		inline static constexpr auto SpriteAnimationPrefix =                "[SpriteAnimation] ";
		inline static constexpr auto EngineLinuxPrefix =                    "[Engine][Linux] ";
		inline static constexpr auto EngineLinuxX11Prefix =                 "[Engine][Linux][X11] ";
		inline static constexpr auto EngineLinuxWaylandPrefix =             "[Engine][Linux][Wayland] ";
		inline static constexpr auto EngineWindowsPrefix =                  "[Engine][Windows] ";
		inline static constexpr auto FileSystemPrefix =                     "[FileSystem] ";
		inline static constexpr auto FileWatcherWindowsPrefix =             "[FileWatcher][Windows] ";
		inline static constexpr auto FileWatcherLinuxPrefix =               "[FileWatcher][Linux] ";
		inline static constexpr auto FileWatcherPrefix =                    "[FileWatcher] ";
		inline static constexpr auto InstrumentorPrefix =                   "[Instrumentor] ";
		inline static constexpr auto LoggerPrefix =                         "[Logger] ";
		inline static constexpr auto LayerStackPrefix =                     "[LayerStack] ";
		inline static constexpr auto InputControllerDirectInputPrefix =     "[Input][Controller][Windows][DirectInput] ";
		inline static constexpr auto InputControllerXInputPrefix =          "[Input][Controller][Windows][XInput] ";
		inline static constexpr auto InputControllerLinuxPrefix =           "[Input][Controller][Linux] ";
		inline static constexpr auto InputControllerPrefix =                "[Input][Controller] ";
		inline static constexpr auto InputWinAPIPrefix =                    "[Input][Windows][WinAPI] ";
		inline static constexpr auto InputPrefix =                          "[Input] ";
		inline static constexpr auto ImagePrefix =                          "[Image] ";
		inline static constexpr auto ImageTGAPrefix =                       "[Image][TGA] ";
		inline static constexpr auto ImagePNGPrefix =                       "[Image][PNG] ";
		inline static constexpr auto ImagePPMPrefix =                       "[Image][PPM] ";
		inline static constexpr auto ImagePNMPrefix =                       "[Image][PNM] ";
		inline static constexpr auto ImagePGMPrefix =                       "[Image][PGM] ";
		inline static constexpr auto ImagePFMPrefix =                       "[Image][PFM] ";
		inline static constexpr auto ImagePAMPrefix =                       "[Image][PAM] ";
		inline static constexpr auto ImageQOIPrefix =                       "[Image][QOI] ";
		inline static constexpr auto ImageBMPPrefix =                       "[Image][BMP] ";
		inline static constexpr auto ImageRadiancePrefix =                  "[Image][Radiance] ";
		inline static constexpr auto Renderer2DPrefix =                     "[Renderer2D] ";
		inline static constexpr auto RendererPrefix =                       "[Renderer] ";
		inline static constexpr auto RendererBufferPrefix =                 "[Renderer][Buffer] ";
		inline static constexpr auto RendererCommandBufferPrefix =          "[Renderer][CommandBuffer] ";
		inline static constexpr auto RendererCommandPoolPrefix =            "[Renderer][CommandPool] ";
		inline static constexpr auto RendererCommandSignaturePrefix =       "[Renderer][CommandSignature] ";
		inline static constexpr auto RendererDescriptorPoolPrefix =         "[Renderer][DescriptorPool] ";
		inline static constexpr auto RendererDescriptorSetPrefix =          "[Renderer][DescriptorSet] ";
		inline static constexpr auto RendererFencePrefix =                  "[Renderer][Fence] ";
		inline static constexpr auto RendererPipelinePrefix =               "[Renderer][Pipeline] ";
		inline static constexpr auto RendererPipelineCachePrefix =          "[Renderer][PipelineCache] ";
		inline static constexpr auto RendererQueuePrefix =                  "[Renderer][Queue] ";
		inline static constexpr auto RendererQueryPoolPrefix =              "[Renderer][QueryPool] ";
		inline static constexpr auto RendererRenderTargetPrefix =           "[Renderer][RenderTarget] ";
		inline static constexpr auto RendererRootSignaturePrefix =          "[Renderer][RootSignature] ";
		inline static constexpr auto RendererSamplerPrefix =                "[Renderer][Sampler] ";
		inline static constexpr auto RendererSemaphorePrefix =              "[Renderer][Semaphore] ";
		inline static constexpr auto RendererSwapChainPrefix =              "[Renderer][SwapChain] ";
		inline static constexpr auto RendererAftermathTrackerPrefix =       "[Renderer][AftermathTracker] ";
		inline static constexpr auto RendererVulkanPrefix =                 "[Renderer][Vulkan] ";
		inline static constexpr auto RendererVulkanReflexPrefix =           "[Renderer][Vulkan][Reflex] ";
		inline static constexpr auto RendererVulkanDevicePrefix =           "[Renderer][Vulkan][Device] ";
		inline static constexpr auto RendererVulkanPhysicalDevicePrefix =   "[Renderer][Vulkan][PhysicalDevice] ";
		inline static constexpr auto RendererVulkanDescriptorPoolPrefix =   "[Renderer][Vulkan][DescriptorPool] ";
		inline static constexpr auto RendererVulkanDescriptorSetPrefix =    "[Renderer][Vulkan][DescriptorSet] ";
		inline static constexpr auto RendererVulkanRenderPassPrefix =       "[Renderer][Vulkan][RenderPass] ";
		inline static constexpr auto RendererVulkanFrameBufferPrefix =      "[Renderer][Vulkan][FrameBuffer] ";
		inline static constexpr auto RendererVulkanRenderTargetPrefix =     "[Renderer][Vulkan][RenderTarget] ";
		inline static constexpr auto RendererVulkanCommandSignaturePrefix = "[Renderer][Vulkan][CommandSignature] ";
		inline static constexpr auto RendererVulkanQueuePrefix =            "[Renderer][Vulkan][Queue] ";
		inline static constexpr auto RendererVulkanCommandPoolPrefix =      "[Renderer][Vulkan][CommandPool] ";
		inline static constexpr auto RendererVulkanCommandBufferPrefix =    "[Renderer][Vulkan][CommandBuffer] ";
		inline static constexpr auto RendererVulkanQueryPoolPrefix =        "[Renderer][Vulkan][QueryPool] ";
		inline static constexpr auto RendererVulkanPipelinePrefix =         "[Renderer][Vulkan][Pipeline] ";
		inline static constexpr auto RendererVulkanPipelineCachePrefix =    "[Renderer][Vulkan][PipelineCache] ";
		inline static constexpr auto RendererVulkanFencePrefix =            "[Renderer][Vulkan][Fence] ";
		inline static constexpr auto RendererVulkanSemaphorePrefix =        "[Renderer][Vulkan][Semaphore] ";
		inline static constexpr auto RendererVulkanBufferPrefix =           "[Renderer][Vulkan][Buffer] ";
		inline static constexpr auto RendererVulkanSamplerPrefix =          "[Renderer][Vulkan][Sampler] ";
		inline static constexpr auto RendererVulkanTexturePrefix =          "[Renderer][Vulkan][Texture] ";
		inline static constexpr auto RendererVulkanSwapChainPrefix =        "[Renderer][Vulkan][SwapChain] ";
		inline static constexpr auto RendererVulkanSurfacePrefix =          "[Renderer][Vulkan][Surface] ";
		inline static constexpr auto RendererVulkanShaderPrefix =           "[Renderer][Vulkan][Shader] ";
		inline static constexpr auto RendererVulkanRootSignaturePrefix =    "[Renderer][Vulkan][RootSignature] ";
		inline static constexpr auto RendererVulkanVMAPrefix =              "[Renderer][Vulkan][VMA] ";
		inline static constexpr auto RendererVulkanInstancePrefix =         "[Renderer][Vulkan][Instance] ";
		inline static constexpr auto RendererVulkanDebugPrefix =            "[Renderer][Vulkan][Debug] ";
		inline static constexpr auto ImGuiPrefix =                          "[ImGui] ";
		inline static constexpr auto NetworkSocketPrefix =                  "[Network][Socket] ";
		inline static constexpr auto NetworkFTPPrefix =                     "[Network][FTP] ";
		inline static constexpr auto NetworkHTTPPrefix =                    "[Network][HTTP] ";
		inline static constexpr auto NetworkTCPListenerPrefix =             "[Network][TCPListener] ";
		inline static constexpr auto NetworkTCPSocketPrefix =               "[Network][TCPSocket] ";
		inline static constexpr auto NetworkUDPSocketPrefix =               "[Network][UDPSocket] ";
		inline static constexpr auto NetworkSocketUnixPrefix =              "[Network][Socket][Unix] ";
		inline static constexpr auto SceneSerializerPrefix =                "[SceneSerializer] ";
		inline static constexpr auto DiscordGameSDKPrefix =                 "[Discord] ";
		inline static constexpr auto SteamworksSDKPrefix =                  "[Steam] ";
		inline static constexpr auto HotReloadingPrefix =                   "[HotReloading] ";
		inline static constexpr auto UtilsStringPrefix =                    "[Utils][String] ";
		inline static constexpr auto UtilsPrefix =                          "[Utils] ";

#ifdef TRAP_PLATFORM_WINDOWS
	private:
		/// <summary>
		/// Retrieves information about the specified console screen buffer.
		/// </summary>
		static void GetInfo() noexcept;
		/// <summary>
		/// Set the new color for the following console output.
		/// </summary>
		/// <param name="wRGBI">New console color.</param>
		static void SetColor(WORD wRGBI) noexcept;
		/// <summary>
		/// Reset the console color to the default for the following output.
		/// </summary>
		static void ResetColor() noexcept;

		static HANDLE m_handleConsole;
		static CONSOLE_SCREEN_BUFFER_INFO m_csbi;
#else
	private:
		/// <summary>
		/// Check whether the terminal supports ANSI color codes.
		/// </summary>
		[[nodiscard]] static bool IsColorTerminal();
#endif
		/// <summary>
		/// Get a time stamp with [HH:MM:SS] format.
		/// </summary>
		/// <returns>Time stamp as a string.</returns>
		[[nodiscard]] static std::string GetTimeStamp();
		/// <summary>
		/// Get a date time stamp with YYYY-MM-DDTHH-MM-SS format.
		/// </summary>
		/// <returns>Time stamp as a string.</returns>
		[[nodiscard]] static std::string GetDateTimeStamp();

		std::vector<std::pair<Level, std::string>> m_buffer{};

		std::mutex m_mtx;

		std::filesystem::path m_path = "trap.log";

		Level m_importance;
	};

	extern Log TRAPLog;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Log::SetImportance(const Level level) noexcept
{
	m_importance = level;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer() const noexcept
{
	return m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Log::Clear() noexcept
{
	m_buffer.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

static constexpr inline TRAP::Log::Level operator|(const TRAP::Log::Level a, const TRAP::Log::Level b) noexcept
{
	return static_cast<TRAP::Log::Level>(static_cast<std::underlying_type<TRAP::Log::Level>::type>(a) |
		static_cast<std::underlying_type<TRAP::Log::Level>::type>(b));
}
static constexpr inline TRAP::Log::Level operator&(const TRAP::Log::Level a, const TRAP::Log::Level b) noexcept
{
	return static_cast<TRAP::Log::Level>(static_cast<std::underlying_type<TRAP::Log::Level>::type>(a) &
		static_cast<std::underlying_type<TRAP::Log::Level>::type>(b));
}
static constexpr inline TRAP::Log::Level operator|=(TRAP::Log::Level& a, const TRAP::Log::Level b) noexcept { return a = (a | b); }
static constexpr inline TRAP::Log::Level operator&=(TRAP::Log::Level& a, const TRAP::Log::Level b) noexcept { return a = (a & b); }

//-------------------------------------------------------------------------------------------------------------------//

#include "Log.inl"

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a trace message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_TRACE(const Args& ... args)
{
	TRAP::TRAPLog.Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)

/// <summary>
/// Log a debug message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_DEBUG(const Args& ... args)
{
	TRAP::TRAPLog.Debug(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#else
/// <summary>
/// Log a debug message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_DEBUG([[maybe_unused]] const Args& ... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a info message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_INFO(const Args& ... args)
{
	TRAP::TRAPLog.Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a warn message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_WARN(const Args& ... args)
{
	TRAP::TRAPLog.Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a error message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_ERROR(const Args& ... args)
{
	TRAP::TRAPLog.Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Log a critical message.
/// </summary>
/// <typeparam name="...Args">Message to log.</typeparam>
template<typename... Args>
void TP_CRITICAL(const Args& ... args)
{
	TRAP::TRAPLog.Critical(args...);
}

#endif /*TRAP_LOG_H*/