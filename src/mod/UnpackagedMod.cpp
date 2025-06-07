#include "UnpackagedMod.h"

#include <regex>

using namespace tudov;

ModConfig tudov::UnpackagedMod::LoadConfig(const std::filesystem::path &directory)
{
	std::ifstream file(directory / "Mod.json");
	if (file)
	{
		nlohmann::json json;
		file >> json;
		tudov::ModConfig config = json.get<tudov::ModConfig>();
		file.close();
	}
	return ModConfig();
}

UnpackagedMod::UnpackagedMod(ModManager &modManager, const std::filesystem::path &directory)
	: Mod(modManager, LoadConfig(directory)),
	  _directory(directory)
{
	auto &&callback = [](const std::filesystem::path &path, const filewatch::Event change_type)
	{
		std::wcout << std::filesystem::absolute(path) << L"\n";
	};

	// bool test = std::filesystem::exists(directory);

	// TODO 未知异常！
	_watcher = std::make_shared<filewatch::FileWatch<std::wstring>>(directory, callback);
}

void UnpackagedMod::Load()
{
	auto &&scriptFilePatterns = std::vector<std::regex>(_config.scripts.files.size());
	for (auto &&pattern : _config.scripts.files)
	{
		scriptFilePatterns.emplace_back(std::regex(pattern));
	}

	for (const auto &entry : std::filesystem::recursive_directory_iterator(_directory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		auto &&fileName = entry.path().filename().string();
		for (auto &&pattern : scriptFilePatterns)
		{
			if (std::regex_match(fileName, pattern))
			{
			}
		}

		// if (fileName == ".lua" || fileName == ".ogg")
		// {
		// 	std::ifstream file(entry.path(), std::ios::binary);
		// 	if (!file)
		// 	{
		// 		std::cerr << "无法打开文件: " << entry.path() << "\n";
		// 		continue;
		// 	}

		// 	std::cout << "读取文件: " << entry.path().filename() << "\n";
		// 	// 示例：读取文件内容到字符串
		// 	std::string content((std::istreambuf_iterator<char>(file)),
		// 						std::istreambuf_iterator<char>());

		// 	// 可以对 content 做进一步处理，比如解析脚本或播放音频
		// }
	}
}

void UnpackagedMod::Unload()
{
}
