#pragma once

#include "Mod.hpp"
#include "ModEntry.hpp"
#include "ScriptEngine.hpp"
#include "ScriptLoader.hpp"
#include "ScriptProvider.hpp"
#include "UnpackagedMod.hpp"
#include "event/EventManager.hpp"
#include "program/EngineComponent.hpp"
#include "util/Defs.hpp"
#include "util/Log.hpp"

#include <filesystem>
#include <vector>

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
		virtual void UnloadMods() = 0;

		virtual std::weak_ptr<Mod> GetLoadedMod(std::string_view namespace_) noexcept = 0;

		virtual std::vector<ModEntry> &GetRequiredMods() noexcept = 0;
		virtual const std::vector<ModEntry> &GetRequiredMods() const noexcept = 0;

		virtual void HotReloadScriptPending(std::string_view scriptName, std::string_view scriptCode, std::string_view scriptNamespace) = 0;

		virtual void Update() = 0;
	};

	class ModManager : public IModManager
	{
	  public:
		enum class ELoadState
		{
			None = 0,
			AllLoading = 1 << 0,
			AllUnloading = 1 << 1,
			HotReloading = 1 << 2,
		};

	  protected:
		using THotReloadScriptsMap = std::unordered_map<std::string, std::tuple<std::string, std::string>>;

	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		ELoadState _loadState;
		UnpackagedMod _virtualUnpackagedMod;

		std::vector<std::filesystem::path> _directories;
		std::vector<std::shared_ptr<Mod>> _loadedMods;
		std::vector<ModEntry> _requiredMods;

		std::unique_ptr<THotReloadScriptsMap> _hotReloadScriptsPending;

	  public:
		void AddMod(const std::filesystem::path &modRoot);

	  public:
		explicit ModManager(Context &context) noexcept;
		~ModManager() noexcept override;

		Context &GetContext() noexcept override;
		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		bool IsNoModMatch() const override;
		bool IsModMatched(const Mod &mod) const override;

		void LoadMods() override;
		void UnloadMods() override;

		std::weak_ptr<Mod> GetLoadedMod(std::string_view namespace_) noexcept override;

		std::vector<ModEntry> &GetRequiredMods() noexcept override;
		const std::vector<ModEntry> &GetRequiredMods() const noexcept override;

		void HotReloadScriptPending(std::string_view scriptName, std::string_view scriptCode, std::string_view scriptNamespace) override;

		void Update() override;

		void InstallToScriptEngine(ScriptEngine &scriptEngine) noexcept;
	};

	ENABLE_ENUM_FLAG_OPERATORS(ModManager::ELoadState);
} // namespace tudov
