#ifndef TRAP_LOG_H
#define TRAP_LOG_H

#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <filesystem>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include "Core/Backports.h"
#include "Core/Types.h"
#include "Utils/Concurrency/LockFreeQueue.h"
#include "Utils/Concurrency/Safe.h"
#include "Utils/Optional.h"

namespace TRAP
{
	enum class LogLevel : u8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::LogLevel operator|(TRAP::LogLevel a, TRAP::LogLevel b) noexcept;
[[nodiscard]] constexpr TRAP::LogLevel operator&(TRAP::LogLevel a, TRAP::LogLevel b) noexcept;
constexpr TRAP::LogLevel operator|=(TRAP::LogLevel& a, TRAP::LogLevel b) noexcept;
constexpr TRAP::LogLevel operator&=(TRAP::LogLevel& a, TRAP::LogLevel b) noexcept;

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP
{
	/// @brief Importance levels for logging.
	enum class LogLevel : u8
	{
		Trace = 0x01u,
		Debug = 0x02u,
		Info = 0x04u,
		Warn = 0x08u,
		Error = 0x10u,
		Critical = 0x20u
	};

	struct LogEntry
	{
		LogLevel Level;
		std::string Message;
		std::chrono::time_point<std::chrono::system_clock> Time = std::chrono::system_clock::now();

		[[nodiscard]] constexpr auto operator<=>(const LogEntry& other) const
		{
			return Time <=> other.Time;
		}
	};

	/// @brief Utility class to log messages to console.
	class Log final
	{
	public:
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
		/// @threadsafe
		[[nodiscard]] std::filesystem::path GetFilePath() const noexcept;
		/// @brief Set the file path used for saving.
		///
		///	Logs files are always saved in the format: "<FileName>-YYYY-MM-DDTHH-MM-SS.<FileEnding>"
		/// @param filePath File path.
		/// @threadsafe
		void SetFilePath(std::filesystem::path filePath) noexcept;

		/// @brief Set the importance level.
		/// Only message with the given LogLevel will be printed to console.
		/// @note All messages are saved in the log file regardless
		///       off the importance level.
		/// @param level LogLevel to use.
		/// @threadsafe
		void SetImportance(LogLevel level) noexcept;

		/// @brief Get the importance level.
		/// @return Currently active importance level.
		/// @threadsafe
		[[nodiscard]] LogLevel GetImportance() const noexcept;

		/// @brief Log a trace message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Trace(Args&& ... args);

		/// @brief Log a debug message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Debug(Args&& ... args);

		/// @brief Log a info message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Info(Args&& ... args);

		/// @brief Log a warn message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Warn(Args&& ... args);

		/// @brief Log a error message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Error(Args&& ... args);

		/// @brief Log a critical message.
		/// @tparam Args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void Critical(Args&& ... args);

		/// @brief Log a message with the provided LogLevel.
		/// @tparam Args Message to log.
		/// @param level LogLevel of the message
		/// @param args Message to log.
		/// @threadsafe
		template<typename... Args>
		constexpr void LogMessage(LogLevel level, Args&&... args);

		/// @brief Get all saved log messages and their associated importance level.
		///        This will also flush the internal buffer, so it will be empty afterwards.
		/// @return Messages with importance level.
		/// @threadsafe
		[[nodiscard]] std::vector<LogEntry> GetBufferAndFlush();

		/// @brief Save all collected messages to file.
		/// @threadsafe
		void Save();
		/// @brief Clears all buffered messages.
		/// @threadsafe
		void Clear() noexcept;

		static constexpr auto WindowVersion =                        "[24w47a2]";
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
		/// @threadsafe
		[[nodiscard]] static std::string GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& time);

		/// @threadsafe
		[[nodiscard]] static constexpr TRAP::Optional<fmt::color> LogLevelToFmtColor(LogLevel level);
#ifdef TRACY_ENABLE
		/// @threadsafe
		[[nodiscard]] static constexpr TRAP::Optional<tracy::Color::ColorType> LogLevelToTracyColor(LogLevel level);
#endif /*TRACY_ENABLE*/

		TRAP::Utils::LockFreeQueue<LogEntry> m_buffer{};
		Utils::Safe<std::filesystem::path, SharedLockableBase(std::shared_mutex)> m_path{};

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		std::atomic<LogLevel> m_importance = LogLevel::Trace | LogLevel::Debug | LogLevel::Info |
		                                     LogLevel::Warn | LogLevel::Error | LogLevel::Critical;
#else
		std::atomic<LogLevel> m_importance = LogLevel::Warn | LogLevel::Error | LogLevel::Critical;
#endif
	};

	/// @brief Retrieve the global logger instance.
	/// @return Global logger instance.
	/// @threadsafe
	[[nodiscard]] TRAP::Log& GetTRAPLog();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::LogLevel operator|(const TRAP::LogLevel a, const TRAP::LogLevel b) noexcept
{
	return static_cast<TRAP::LogLevel>(std::to_underlying(a) | std::to_underlying(b));
}
[[nodiscard]] constexpr TRAP::LogLevel operator&(const TRAP::LogLevel a, const TRAP::LogLevel b) noexcept
{
	return static_cast<TRAP::LogLevel>(std::to_underlying(a) & std::to_underlying(b));
}
constexpr TRAP::LogLevel operator|=(TRAP::LogLevel& a, const TRAP::LogLevel b) noexcept
{
	return a = (a | b);
}
constexpr TRAP::LogLevel operator&=(TRAP::LogLevel& a, const TRAP::LogLevel b) noexcept
{
	return a = (a & b);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::LogLevel>
{
	static constexpr auto parse(const fmt::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	static fmt::format_context::iterator format(const TRAP::LogLevel logLevel, fmt::format_context& ctx)
	{
		std::string enumStr{};
		switch(logLevel)
		{
		case TRAP::LogLevel::Trace:
			enumStr = "Trace";
			break;
		case TRAP::LogLevel::Debug:
			enumStr = "Debug";
			break;
		case TRAP::LogLevel::Info:
			enumStr = "Info";
			break;
		case TRAP::LogLevel::Warn:
			enumStr = "Warn";
			break;
		case TRAP::LogLevel::Error:
			enumStr = "Error";
			break;
		case TRAP::LogLevel::Critical:
			enumStr = "Critical";
			break;
		}

		return fmt::format_to(ctx.out(), "{}", enumStr);
	}
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
constexpr void TRAP::Log::Trace(Args&& ... args)
{
	LogMessage(LogLevel::Trace, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::Debug(Args&& ... args)
{
	LogMessage(LogLevel::Debug, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::Info(Args&& ... args)
{
	LogMessage(LogLevel::Info, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::Warn(Args&& ... args)
{
	LogMessage(LogLevel::Warn, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::Error(Args&& ... args)
{
	LogMessage(LogLevel::Error, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::Critical(Args&& ... args)
{
	LogMessage(LogLevel::Critical, std::forward<Args>(args)...);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Optional<fmt::color> TRAP::Log::LogLevelToFmtColor(const LogLevel level)
{
	switch(level)
	{
	case LogLevel::Trace:
		return fmt::color::magenta;
	case LogLevel::Debug:
		return fmt::color::cyan;
	case LogLevel::Info:
		return fmt::color::green;
	case LogLevel::Warn:
		return fmt::color::yellow;
	case LogLevel::Error:
		return fmt::color::red;
	case LogLevel::Critical:
		return fmt::color::dark_red;
	}

	return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
[[nodiscard]] constexpr TRAP::Optional<tracy::Color::ColorType> TRAP::Log::LogLevelToTracyColor(const LogLevel level)
{
	switch(level)
	{
	case LogLevel::Trace:
		return tracy::Color::ColorType::Magenta;
	case LogLevel::Debug:
		return tracy::Color::ColorType::Cyan;
	case LogLevel::Info:
		return tracy::Color::ColorType::Green;
	case LogLevel::Warn:
		return tracy::Color::ColorType::Yellow;
	case LogLevel::Error:
		return tracy::Color::ColorType::Red;
	case LogLevel::Critical:
		return tracy::Color::ColorType::DarkRed;
	}

	return TRAP::NullOpt;
}
#endif /*TRACY_ENABLE*/

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TRAP::Log::LogMessage(const LogLevel level, Args&&... args)
{
	if(!std::is_constant_evaluated())
	{
		const auto logTime = std::chrono::system_clock::now();

		const std::string logMsg = fmt::format("{}[{}]{}", GetTimeStamp(logTime), level, (fmt::format("{}", args) + ...));

	#if !defined(TRAP_RELEASE)
		if (std::to_underlying(m_importance & level) != 0u)
		{
			const auto fmtColor = LogLevelToFmtColor(level);
			const bool isError = std::to_underlying(level & LogLevel::Error) != 0u ||
								std::to_underlying(level & LogLevel::Critical) != 0u;

			fmt::print(isError ? std::cerr : std::cout, "{}\n", fmt::styled(logMsg, fmtColor ? fmt::fg(*fmtColor) : fmt::text_style{}));
		}
	#endif

	#ifdef TRACY_ENABLE
		const auto tracyColor = LogLevelToTracyColor(level);
		if(tracyColor)
			TracyMessageC(logMsg.c_str(), logMsg.size(), *tracyColor);
		else
			TracyMessage(logMsg.c_str(), logMsg.size());
	#endif

		m_buffer.Push({.Level = level, .Message = logMsg, .Time = logTime});
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a trace message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_TRACE(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Trace(args...);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a debug message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Debug(args...);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a info message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_INFO(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Info(args...);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a warn message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_WARN(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Warn(args...);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a error message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_ERROR(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Error(args...);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Log a critical message.
/// @tparam Args Message to log.
/// @threadsafe
template<typename... Args>
constexpr void TP_CRITICAL(const Args& ... args)
{
	if(!std::is_constant_evaluated())
	{
		TRAP::GetTRAPLog().Critical(args...);
	}
}

#endif /*TRAP_LOG_H*/
