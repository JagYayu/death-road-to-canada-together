/**
 * @file mod/ModManager.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

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
#include "system/Log.hpp"

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

		[[nodiscard]] virtual bool IsNoModMatch() const = 0;
		[[nodiscard]] virtual bool IsModMatched(const Mod &mod) const = 0;

		virtual void LoadMods() = 0;
		virtual void LoadModsDeferred() = 0;
		virtual void UnloadMods() = 0;
		virtual void UnloadModsDeferred() = 0;

		[[nodiscard]] virtual bool IsModAvailable(std::string_view modUID) const noexcept = 0;
		[[nodiscard]] virtual bool IsModAvailable(std::string_view modUID, const Version &version) const noexcept = 0;
		[[nodiscard]] virtual std::vector<ModListedEntry> ListAvailableMods() const noexcept = 0;

		[[nodiscard]] virtual std::shared_ptr<Mod> FindLoadedMod(std::string_view modUID) noexcept = 0;

		[[nodiscard]] virtual std::vector<ModRequirement> &GetRequiredMods() noexcept = 0;
		[[nodiscard]] virtual const std::vector<ModRequirement> &GetRequiredMods() const noexcept = 0;

		[[nodiscard]] virtual bool HasUpdateScriptPending() noexcept = 0;
		virtual void UpdateScriptPending(std::string_view scriptName, TextID scriptTextID, std::string_view scriptModUID) = 0;

		virtual void Update() = 0;
	};

	class LuaBindings;
	class UnpackagedMod;

	class ModManager : public IModManager, public IDebugProvider, private ILogProvider
	{
		friend LuaBindings;

	  private:
		struct ModDirectory
		{
			EGlobalStorageLocation location;
			std::filesystem::path path;
		};

		enum class ELoadState : std::uint8_t
		{
			None = 0,
			LoadPending = 1 << 0,
			Loading = 1 << 1,
			UnloadPending = 1 << 2,
			Unloading = 1 << 3,
			HotReloading = 1 << 4,
		};

	  protected:
		//                                             scriptName              textID  modUID
		using HotReloadScriptsMap = std::unordered_map<std::string, std::tuple<TextID, std::string_view>>;

	  private:
		std::shared_ptr<Log> _log;
		Context &_context;
		ELoadState _loadState;
		std::unique_ptr<UnpackagedMod> _virtualUnpackagedMod;

		std::vector<ModDirectory> _modDirectories;
		std::vector<std::shared_ptr<Mod>> _loadedMods;
		std::vector<ModRequirement> _requiredMods;

		std::unique_ptr<HotReloadScriptsMap> _updateScriptsPending;

	  public:
		explicit ModManager(Context &context) noexcept;
		~ModManager() noexcept override;

	  public:
		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		void Initialize() noexcept override;
		void Deinitialize() noexcept override;

		[[nodiscard]] bool IsNoModMatch() const override;
		[[nodiscard]] bool IsModMatched(const Mod &mod) const override;

		void LoadMods() override;
		void LoadModsDeferred() noexcept override;
		void UnloadMods() override;
		void UnloadModsDeferred() noexcept override;

		[[nodiscard]] bool IsModAvailable(std::string_view modUID) const noexcept override;
		[[nodiscard]] bool IsModAvailable(std::string_view modUID, const Version &version) const noexcept override;
		[[nodiscard]] std::vector<ModListedEntry> ListAvailableMods() const noexcept override;

		[[nodiscard]] std::shared_ptr<Mod> FindLoadedMod(std::string_view namespace_) noexcept override;

		[[nodiscard]] std::vector<ModRequirement> &GetRequiredMods() noexcept override;
		[[nodiscard]] const std::vector<ModRequirement> &GetRequiredMods() const noexcept override;

		[[nodiscard]] virtual bool HasUpdateScriptPending() noexcept override;
		void UpdateScriptPending(std::string_view scriptName, TextID scriptTextID, std::string_view scriptModUID) override;

		void Update() override;

		void ProvideDebug(IDebugManager &debugManager) noexcept override;

	  private:
		bool IsModAvailableImpl(std::string_view modUID, const Version *version) const noexcept;
		void UpdateScripts() noexcept;

		std::vector<DebugConsoleResult> DebugAdd(std::string_view arg);
	};
} // namespace tudov
