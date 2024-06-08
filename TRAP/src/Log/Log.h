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

#include "Core/Backports.h"
#include "Maths/Types.h"

namespace TRAP
{
	/// @brief Utility class to log messages to console.
	class Log final
	{
	public:
		/// @brief Constructor.
		explicit Log();
		/// @brief Constructor.
		/// @param filePath Path for the log file.
		explicit Log(std::filesystem::path filePath);
		///	@brief Destructor.
		~Log();
		/// @brief Copy constructor.
		consteval Log(const Log&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Log& operator=(const Log&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Log(Log&&) noexcept = delete;
		/// @brief Move assignment operator.
		constexpr Log& operator=(Log&&) noexcept = delete;

		/// @brief Get the current used file path for saving.
		/// @return File path.
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// @brief Set the file path used for saving.
		///
		///	Logs files are always saved in the format: "<FileName>-YYYY-MM-DDTHH-MM-SS.<FileEnding>"
		/// @param filePath File path.
		void SetFilePath(std::filesystem::path filePath) noexcept;

		/// @brief Importance levels.
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

		/// @brief Set the importance level for the log messages.
		/// Messages that are blow the given importance level
		/// won't be printed to the console.
		/// @note All messages are saved in the log file regardless
		///       off the importance level.
		/// @param level Importance level to use.
		constexpr void SetImportance(Level level) noexcept;

		/// @brief Log a trace message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Trace(Args&& ... args);

		/// @brief Log a debug message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Debug(Args&& ... args);

		/// @brief Log a info message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Info(Args&& ... args);

		/// @brief Log a warn message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Warn(Args&& ... args);

		/// @brief Log a error message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Error(Args&& ... args);

		/// @brief Log a critical message.
		/// @tparam Args Message to log.
		template<typename... Args>
		void Critical(Args&& ... args);

		/// @brief Get all saved log messages and their associated importance level.
		/// @return Messages with importance level.
		[[nodiscard]] constexpr const std::vector<std::pair<Level, std::string>>& GetBuffer() const noexcept;

		/// @brief Save all collected messages to file.
		void Save() const;
		/// @brief Clears all buffered messages.
		constexpr void Clear() noexcept;

		static constexpr auto WindowVersion =                        "[24w23b3]";
		static constexpr auto WindowPrefix =                         "[Window] ";
		static constexpr auto WindowIconPrefix =                     "[Window][Icon] ";
		static constexpr auto ConfigPrefix =                         "[Config] ";
		static constexpr auto ApplicationPrefix =                    "[Application] ";
		static constexpr auto ShaderManagerPrefix =                  "[ShaderManager] ";
		static constexpr auto ShaderPrefix =                         "[Shader] ";
		static constexpr auto ShaderGLSLPrefix =                     "[Shader][GLSL] ";
		static constexpr auto ShaderSPIRVPrefix =                    "[Shader][SPIR-V] ";
		static constexpr auto TextureManagerPrefix =                 "[TextureManager] ";
		static constexpr auto TextureManagerTexture2DPrefix =        "[TextureManager][Texture2D] ";
		static constexpr auto TextureManagerTextureCubePrefix =      "[TextureManager][TextureCube] ";
		static constexpr auto TexturePrefix =                        "[Texture] ";
		static constexpr auto SpriteManagerPrefix =                  "[SpriteManager] ";
		static constexpr auto SpriteAnimationPrefix =                "[SpriteAnimation] ";
		static constexpr auto EngineLinuxPrefix =                    "[Engine][Linux] ";
		static constexpr auto EngineLinuxX11Prefix =                 "[Engine][Linux][X11] ";
		static constexpr auto EngineLinuxWaylandPrefix =             "[Engine][Linux][Wayland] ";
		static constexpr auto EngineWindowsPrefix =                  "[Engine][Windows] ";
		static constexpr auto FileSystemPrefix =                     "[FileSystem] ";
		static constexpr auto FileWatcherWindowsPrefix =             "[FileWatcher][Windows] ";
		static constexpr auto FileWatcherLinuxPrefix =               "[FileWatcher][Linux] ";
		static constexpr auto FileWatcherPrefix =                    "[FileWatcher] ";
		static constexpr auto InstrumentorPrefix =                   "[Instrumentor] ";
		static constexpr auto LoggerPrefix =                         "[Logger] ";
		static constexpr auto LayerStackPrefix =                     "[LayerStack] ";
		static constexpr auto InputControllerDirectInputPrefix =     "[Input][Controller][Windows][DirectInput] ";
		static constexpr auto InputControllerXInputPrefix =          "[Input][Controller][Windows][XInput] ";
		static constexpr auto InputControllerLinuxPrefix =           "[Input][Controller][Linux] ";
		static constexpr auto InputControllerPrefix =                "[Input][Controller] ";
		static constexpr auto InputWinAPIPrefix =                    "[Input][Windows][WinAPI] ";
		static constexpr auto InputPrefix =                          "[Input] ";
		static constexpr auto ImagePrefix =                          "[Image] ";
		static constexpr auto ImageTGAPrefix =                       "[Image][TGA] ";
		static constexpr auto ImagePNGPrefix =                       "[Image][PNG] ";
		static constexpr auto ImagePPMPrefix =                       "[Image][PPM] ";
		static constexpr auto ImagePNMPrefix =                       "[Image][PNM] ";
		static constexpr auto ImagePGMPrefix =                       "[Image][PGM] ";
		static constexpr auto ImagePFMPrefix =                       "[Image][PFM] ";
		static constexpr auto ImagePAMPrefix =                       "[Image][PAM] ";
		static constexpr auto ImageQOIPrefix =                       "[Image][QOI] ";
		static constexpr auto ImageBMPPrefix =                       "[Image][BMP] ";
		static constexpr auto ImageRadiancePrefix =                  "[Image][Radiance] ";
		static constexpr auto Renderer2DPrefix =                     "[Renderer2D] ";
		static constexpr auto RendererPrefix =                       "[Renderer] ";
		static constexpr auto RendererBufferPrefix =                 "[Renderer][Buffer] ";
		static constexpr auto RendererCommandBufferPrefix =          "[Renderer][CommandBuffer] ";
		static constexpr auto RendererCommandPoolPrefix =            "[Renderer][CommandPool] ";
		static constexpr auto RendererCommandSignaturePrefix =       "[Renderer][CommandSignature] ";
		static constexpr auto RendererDescriptorPoolPrefix =         "[Renderer][DescriptorPool] ";
		static constexpr auto RendererDescriptorSetPrefix =          "[Renderer][DescriptorSet] ";
		static constexpr auto RendererFencePrefix =                  "[Renderer][Fence] ";
		static constexpr auto RendererPipelinePrefix =               "[Renderer][Pipeline] ";
		static constexpr auto RendererPipelineCachePrefix =          "[Renderer][PipelineCache] ";
		static constexpr auto RendererQueuePrefix =                  "[Renderer][Queue] ";
		static constexpr auto RendererQueryPoolPrefix =              "[Renderer][QueryPool] ";
		static constexpr auto RendererRenderTargetPrefix =           "[Renderer][RenderTarget] ";
		static constexpr auto RendererRootSignaturePrefix =          "[Renderer][RootSignature] ";
		static constexpr auto RendererSamplerPrefix =                "[Renderer][Sampler] ";
		static constexpr auto RendererSemaphorePrefix =              "[Renderer][Semaphore] ";
		static constexpr auto RendererSwapChainPrefix =              "[Renderer][SwapChain] ";
		static constexpr auto RendererAftermathTrackerPrefix =       "[Renderer][AftermathTracker] ";
		static constexpr auto RendererVulkanPrefix =                 "[Renderer][Vulkan] ";
		static constexpr auto RendererVulkanReflexPrefix =           "[Renderer][Vulkan][Reflex] ";
		static constexpr auto RendererVulkanDevicePrefix =           "[Renderer][Vulkan][Device] ";
		static constexpr auto RendererVulkanPhysicalDevicePrefix =   "[Renderer][Vulkan][PhysicalDevice] ";
		static constexpr auto RendererVulkanDescriptorPoolPrefix =   "[Renderer][Vulkan][DescriptorPool] ";
		static constexpr auto RendererVulkanDescriptorSetPrefix =    "[Renderer][Vulkan][DescriptorSet] ";
		static constexpr auto RendererVulkanRenderPassPrefix =       "[Renderer][Vulkan][RenderPass] ";
		static constexpr auto RendererVulkanFrameBufferPrefix =      "[Renderer][Vulkan][FrameBuffer] ";
		static constexpr auto RendererVulkanRenderTargetPrefix =     "[Renderer][Vulkan][RenderTarget] ";
		static constexpr auto RendererVulkanCommandSignaturePrefix = "[Renderer][Vulkan][CommandSignature] ";
		static constexpr auto RendererVulkanQueuePrefix =            "[Renderer][Vulkan][Queue] ";
		static constexpr auto RendererVulkanCommandPoolPrefix =      "[Renderer][Vulkan][CommandPool] ";
		static constexpr auto RendererVulkanCommandBufferPrefix =    "[Renderer][Vulkan][CommandBuffer] ";
		static constexpr auto RendererVulkanQueryPoolPrefix =        "[Renderer][Vulkan][QueryPool] ";
		static constexpr auto RendererVulkanPipelinePrefix =         "[Renderer][Vulkan][Pipeline] ";
		static constexpr auto RendererVulkanPipelineCachePrefix =    "[Renderer][Vulkan][PipelineCache] ";
		static constexpr auto RendererVulkanFencePrefix =            "[Renderer][Vulkan][Fence] ";
		static constexpr auto RendererVulkanSemaphorePrefix =        "[Renderer][Vulkan][Semaphore] ";
		static constexpr auto RendererVulkanBufferPrefix =           "[Renderer][Vulkan][Buffer] ";
		static constexpr auto RendererVulkanSamplerPrefix =          "[Renderer][Vulkan][Sampler] ";
		static constexpr auto RendererVulkanTexturePrefix =          "[Renderer][Vulkan][Texture] ";
		static constexpr auto RendererVulkanSwapChainPrefix =        "[Renderer][Vulkan][SwapChain] ";
		static constexpr auto RendererVulkanSurfacePrefix =          "[Renderer][Vulkan][Surface] ";
		static constexpr auto RendererVulkanShaderPrefix =           "[Renderer][Vulkan][Shader] ";
		static constexpr auto RendererVulkanRootSignaturePrefix =    "[Renderer][Vulkan][RootSignature] ";
		static constexpr auto RendererVulkanVMAPrefix =              "[Renderer][Vulkan][VMA] ";
		static constexpr auto RendererVulkanInstancePrefix =         "[Renderer][Vulkan][Instance] ";
		static constexpr auto RendererVulkanDebugPrefix =            "[Renderer][Vulkan][Debug] ";
		static constexpr auto ImGuiPrefix =                          "[ImGui] ";
		static constexpr auto NetworkSocketPrefix =                  "[Network][Socket] ";
		static constexpr auto NetworkFTPPrefix =                     "[Network][FTP] ";
		static constexpr auto NetworkHTTPPrefix =                    "[Network][HTTP] ";
		static constexpr auto NetworkTCPListenerPrefix =             "[Network][TCPListener] ";
		static constexpr auto NetworkTCPSocketPrefix =               "[Network][TCPSocket] ";
		static constexpr auto NetworkUDPSocketPrefix =               "[Network][UDPSocket] ";
		static constexpr auto NetworkSocketUnixPrefix =              "[Network][Socket][Unix] ";
		static constexpr auto SceneSerializerPrefix =                "[SceneSerializer] ";
		static constexpr auto DiscordGameSDKPrefix =                 "[Discord] ";
		static constexpr auto SteamworksSDKPrefix =                  "[Steam] ";
		static constexpr auto HotReloadingPrefix =                   "[HotReloading] ";
		static constexpr auto UtilsDBusPrefix =                      "[Utils][DBus] ";
		static constexpr auto UtilsStringPrefix =                    "[Utils][String] ";
		static constexpr auto UtilsPrefix =                          "[Utils] ";

	private:
		/// @brief Get a time stamp with [HH:MM:SS] format.
		/// @return Time stamp as a string.
		[[nodiscard]] static std::string GetTimeStamp();
		/// @brief Get a date time stamp with YYYY-MM-DDTHH-MM-SS format.
		/// @return Time stamp as a string.
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

static constexpr TRAP::Log::Level operator|(const TRAP::Log::Level a, const TRAP::Log::Level b) noexcept
{
	return static_cast<TRAP::Log::Level>(std::to_underlying(a) | std::to_underlying(b));
}
static constexpr TRAP::Log::Level operator&(const TRAP::Log::Level a, const TRAP::Log::Level b) noexcept
{
	return static_cast<TRAP::Log::Level>(std::to_underlying(a) & std::to_underlying(b));
}
static constexpr TRAP::Log::Level operator|=(TRAP::Log::Level& a, const TRAP::Log::Level b) noexcept { return a = (a | b); }
static constexpr TRAP::Log::Level operator&=(TRAP::Log::Level& a, const TRAP::Log::Level b) noexcept { return a = (a & b); }

//-------------------------------------------------------------------------------------------------------------------//

#include "Log.inl"

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a trace message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_TRACE(const Args& ... args)
{
	TRAP::TRAPLog.Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)

/// @brief Log a debug message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_DEBUG(const Args& ... args)
{
	TRAP::TRAPLog.Debug(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#else
/// @brief Log a debug message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_DEBUG([[maybe_unused]] const Args& ... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a info message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_INFO(const Args& ... args)
{
	TRAP::TRAPLog.Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a warn message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_WARN(const Args& ... args)
{
	TRAP::TRAPLog.Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a error message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_ERROR(const Args& ... args)
{
	TRAP::TRAPLog.Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a critical message.
/// @tparam Args Message to log.
template<typename... Args>
void TP_CRITICAL(const Args& ... args)
{
	TRAP::TRAPLog.Critical(args...);
}

#endif /*TRAP_LOG_H*/
