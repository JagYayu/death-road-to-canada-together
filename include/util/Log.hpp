#pragma once

#include "Micros.hpp"
#include "program/Tudov.hpp"

#include <json.hpp>
#include <queue>
#include <sol/lua_value.hpp>

#include <atomic>
#include <mutex>

namespace tudov
{
	class LuaAPI;

	class Log
	{
		friend LuaAPI;

	  private:
		struct Entry
		{
			std::chrono::system_clock::time_point time;
			std::string_view verbosity;
			std::string module;
			std::string message;
		};

	  public:
		enum class EVerbosity : std::int8_t
		{
			All = -1,
			None = 0,
			Fatal = 1 << 0,
			Error = 1 << 1,
			Warn = 1 << 2,
			Info = 1 << 3,
			Debug = 1 << 4,
			Trace = 1 << 5,
		};

	  private:
		static constexpr decltype(auto) VerbTrace = "Trace";
		static constexpr decltype(auto) VerbInfo = "Info";
		static constexpr decltype(auto) VerbDebug = "Debug";
		static constexpr decltype(auto) VerbWarn = "Warn";
		static constexpr decltype(auto) VerbError = "Error";
		static constexpr decltype(auto) VerbFatal = "Fatal";

	  private:
		static EVerbosity _globalVerbosities;
		static std::unordered_map<std::string, EVerbosity> _verbosities;
		static std::unordered_map<std::string, EVerbosity> _verbositiesOverrides;
		static std::unordered_map<std::string, std::shared_ptr<Log>> _logInstances;
		static std::queue<Entry> _queue;
		static std::mutex _mutex;
		static std::condition_variable _cv;
		static std::atomic<bool> _exit;

	  private:
		static void Process();
		static void OutputImpl(std::string_view module, std::string_view verb, std::string_view str);

	  public:
		static std::shared_ptr<Log> Get(std::string_view module) noexcept;
		static Log &GetInstance() noexcept;
		static void CleanupExpired() noexcept;
		static void Quit() noexcept;
		static std::optional<EVerbosity> GetVerbosity(const std::string &module) noexcept;
		static bool SetVerbosity(const std::string &module, EVerbosity verb) noexcept;
		static void UpdateVerbosities(const nlohmann::json &config);
		static std::optional<EVerbosity> GetVerbosityOverride(const std::string &module) noexcept;
		static void SetVerbosityOverride(const std::string &module, EVerbosity verb) noexcept;
		static void Exit() noexcept;

		static std::size_t CountLogs() noexcept;
		static std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator BeginLogs() noexcept;
		static std::unordered_map<std::string, std::shared_ptr<Log>>::const_iterator EndLogs() noexcept;
		static std::size_t CountVerbosities() noexcept;
		static std::unordered_map<std::string, EVerbosity>::const_iterator BeginVerbosities() noexcept;
		static std::unordered_map<std::string, EVerbosity>::const_iterator EndVerbosities() noexcept;
		static std::size_t CountVerbositiesOverrides() noexcept;
		static std::unordered_map<std::string, EVerbosity>::const_iterator BeginVerbositiesOverrides() noexcept;
		static std::unordered_map<std::string, EVerbosity>::const_iterator EndVerbositiesOverrides() noexcept;

	  private:
		std::string _module;

		bool CanOutput(EVerbosity verb) const noexcept;
		void Output(std::string_view verb, std::string_view str) const noexcept;

	  public:
		explicit Log(const std::string &module) noexcept;
		~Log() noexcept;

		const std::string &GetModule() const;

		TE_FORCEINLINE EVerbosity GetVerbosity() const noexcept
		{
			auto &&verb = GetVerbosity(_module);
			return verb.has_value() ? verb.value() : Log::EVerbosity::None;
		}

		TE_FORCEINLINE bool CanTrace() const noexcept
		{
			return CanOutput(EVerbosity::Trace);
		}
		TE_FORCEINLINE bool CanInfo() const noexcept
		{
			return CanOutput(EVerbosity::Info);
		}
		TE_FORCEINLINE bool CanDebug() const noexcept
		{
			return CanOutput(EVerbosity::Debug);
		}
		TE_FORCEINLINE bool CanWarn() const noexcept
		{
			return CanOutput(EVerbosity::Warn);
		}
		TE_FORCEINLINE bool CanError() const noexcept
		{
			return CanOutput(EVerbosity::Error);
		}
		TE_FORCEINLINE bool CanFatal() const noexcept
		{
			return CanOutput(EVerbosity::Fatal);
		}

		template <typename... Args>
		TE_FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			Output(VerbTrace, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TE_FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			Output(VerbDebug, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TE_FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			Output(VerbInfo, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TE_FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			Output(VerbWarn, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TE_FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			Output(VerbError, std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		TE_FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args) noexcept
		{
			auto &&str = std::format(fmt, std::forward<Args>(args)...);
			Output(VerbFatal, str);
			Tudov::FatalError(str);
		}
	};

	struct ILogProvider
	{
		virtual ~ILogProvider() noexcept = default;

		virtual Log &GetLog() noexcept = 0;

		const Log &GetLog() const noexcept
		{
			return const_cast<ILogProvider *>(this)->GetLog();
		}

		const std::string &GetModule() const;

		TE_FORCEINLINE Log::EVerbosity GetVerbosity() const
		{
			return GetLog().GetVerbosity();
		}

		TE_FORCEINLINE bool CanTrace() const
		{
			return GetLog().CanTrace();
		}
		TE_FORCEINLINE bool CanInfo() const
		{
			return GetLog().CanInfo();
		}
		TE_FORCEINLINE bool CanDebug() const
		{
			return GetLog().CanDebug();
		}
		TE_FORCEINLINE bool CanWarn() const
		{
			return GetLog().CanWarn();
		}
		TE_FORCEINLINE bool CanError() const
		{
			return GetLog().CanError();
		}
		TE_FORCEINLINE bool CanFatal() const
		{
			return GetLog().CanFatal();
		}

		template <typename... Args>
		TE_FORCEINLINE void Trace(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Trace(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TE_FORCEINLINE void Debug(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Debug(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TE_FORCEINLINE void Info(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Info(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TE_FORCEINLINE void Warn(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Warn(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TE_FORCEINLINE void Error(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Error(fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		TE_FORCEINLINE void Fatal(std::format_string<Args...> fmt, Args &&...args)
		{
			GetLog().Fatal(fmt, std::forward<Args>(args)...);
		}
	};
} // namespace tudov
