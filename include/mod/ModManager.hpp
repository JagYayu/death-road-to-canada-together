#pragma once

#include "Mod.hpp"
#include "ModListedEntry.hpp"
#include "ModRequirement.hpp"
#include "ScriptEngine.hpp"
#include "ScriptProvider.hpp"
#include "UnpackagedMod.hpp"
#include "debug/Debug.hpp"
#include "event/EventManager.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <filesystem>

namespace tudov
{
	class Context;

	struct IModManager : public IEngineComponent
	{
		virtual ~IModManager() noexcept = default;

		virtual void Initialize() noexcept = 0;
		virtual void Deinitialize() noexcept = 0;

		virtual bool IsNoModMatch() const = 0;
		virtual bool IsModMatched(const Mod &mod) const = 0;

		virtual void LoadMods() = 0;
		virtual void LoadModsDeferred() = 0;
		virtual void UnloadMods() = 0;

		virtual std::vector<ModListedEntry> ListAvailableMods() noexcept = 0;

		virtual std::weak_ptr<Mod> FindLoadedMod(std::string_view uid) noexcept = 0;

		virtual std::vector<ModRequirement> &GetRequiredMods() noexcept = 0;
		virtual const std::vector<ModRequirement> &GetRequiredMods() const noexcept = 0;

		virtual void HotReloadScriptPending(std::string_view scriptName, std::string_view scriptCode, std::string_view scriptNamespace) = 0;

		virtual void Update() = 0;
	};

	class LuaAPI;

	class ModManager : public IModManager, public IDebugProvider
	{
		friend LuaAPI;

	  public:
		enum class ELoadState
		{
			None = 0,
			LoadPending = 1 << 0,
			Loading = 1 << 1,
			Unloading = 1 << 2,
			HotReloading = 1 << 3,
		};

	  protected:
		using HotReloadScriptsMap = std::unordered_map<std::string, std::tuple<std::string, std::string>>;

	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		ELoadState _loadState;
		UnpackagedMod _virtualUnpackagedMod;

		std::vector<std::filesystem::path> _directories;
		std::vector<std::shared_ptr<Mod>> _loadedMods;
		std::vector<ModRequirement> _requiredMods;

		std::unique_ptr<HotReloadScriptsMap> _hotReloadScriptsPending;

	  public:
		explicit ModManager(Context &context) noexcept;
		~ModManager() noexcept override;

	  private:
		std::vector<DebugConsoleResult> DebugAdd(std::string_view arg);

	  public:
		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		bool IsNoModMatch() const override;
		bool IsModMatched(const Mod &mod) const override;

		void LoadMods() override;
		void LoadModsDeferred() override;
		void UnloadMods() override;

		std::vector<ModListedEntry> ListAvailableMods() noexcept override;

		std::weak_ptr<Mod> FindLoadedMod(std::string_view namespace_) noexcept override;

		std::vector<ModRequirement> &GetRequiredMods() noexcept override;
		const std::vector<ModRequirement> &GetRequiredMods() const noexcept override;

		void HotReloadScriptPending(std::string_view scriptName, std::string_view scriptCode, std::string_view scriptNamespace) override;

		void Update() override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;
	};
} // namespace tudov
