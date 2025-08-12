#ifdef _WIN32

#	include "util/SystemUtils.hpp"
#	include "util/Log.hpp"

#	include <windows.h>
#	include <filesystem>

using namespace tudov;

// region Clipboard

bool SystemUtils::IsClipboardAvailable() noexcept
{
	return true;
}

void SystemUtils::CopyToClipboard(const std::string_view text) noexcept
{
	if (OpenClipboard(nullptr))
	{
		EmptyClipboard();
		HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
		if (mem)
		{
			memcpy(GlobalLock(mem), text.data(), text.size() + 1);
			GlobalUnlock(mem);
			SetClipboardData(CF_TEXT, mem);
		}
		CloseClipboard();
	}
}

void SystemUtils::ClearClipboard() noexcept
{
	EmptyClipboard();
}

// endregion

// region Script Editor

bool SystemUtils::IsScriptEditorAvailable() noexcept
{
	return true;
}

void SystemUtils::OpenScriptEditor(std::filesystem::path path, std::uint32_t line)
{
	if (!std::filesystem::exists(path))
	{
		throw std::runtime_error("Script file does not exist");
	}

	std::string command = std::format("code --reuse-window --goto \"{}:{}\"",
	                                  std::filesystem::path(path).string(),
	                                  line);

	int result = system(command.c_str());
	if (result != 0)
	{
		throw std::runtime_error("Failed to open VSCode");
	}
}

// endregion Script Editor

#endif
