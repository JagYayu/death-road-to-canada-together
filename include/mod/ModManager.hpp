#pragma once

#include "Mod.hpp"
#include "ModListedEntry.hpp"
#include "ModRequirement.hpp"
#include "ScriptEngine.hpp"
#include "ScriptProvider.hpp"
#include "data/GlobalStorageLocation.hpp"
#include "debug/Debug.hpp"
#include "event/EventManager.hpp"
#include "program/EngineComponent.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <memory>

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

		virtual void UpdateScriptPending(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view scriptNamespace) = 0;

		virtual void Update() = 0;
	};

	class LuaAPI;
	class UnpackagedMod;

	class ModManager : public IModManager, public IDebugProvider
	{
		friend LuaAPI;

	  private:
		struct ModDirectory
		{
			EGlobalStorageLocation location;
			std::filesystem::path path;
		};

	  public:
		enum class ELoadState : std::uint8_t
		{
			None = 0,
			LoadPending = 1 << 0,
			Loading = 1 << 1,
			Unloading = 1 << 2,
			HotReloading = 1 << 3,
		};

	  protected:
		using HotReloadScriptsMap = std::unordered_map<std::string, std::tuple<std::shared_ptr<TextResource>, std::string>>;

	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		ELoadState _loadState;
		std::unique_ptr<UnpackagedMod> _virtualUnpackagedMod;

		std::vector<std::filesystem::path> _directories;
		std::vector<ModDirectory> _modDirectories;
		std::vector<std::shared_ptr<Mod>> _loadedMods;
		std::vector<ModRequirement> _requiredMods;

		std::unique_ptr<HotReloadScriptsMap> _updateScriptsPending;

	  public:
		explicit ModManager(Context &context) noexcept;
		~ModManager() noexcept override;

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

		void UpdateScriptPending(std::string_view scriptName, const std::shared_ptr<TextResource> &scriptCode, std::string_view scriptModUID) override;

		void Update() override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;

	  private:
		void UpdateScripts() noexcept;

		std::vector<DebugConsoleResult> DebugAdd(std::string_view arg);
	};
} // namespace tudov
