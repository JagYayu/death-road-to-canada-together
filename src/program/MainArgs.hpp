#pragma once

#include <string>
#include <vector>

namespace tudov
{
	class MainArgs
	{
	private:
		std::vector<std::string> args;

	public:
		explicit MainArgs(int argc, char **argv);

		size_t size() const;

		const std::string &operator[](size_t i) const { return args[i]; }

		auto begin() const;
		auto end() const;
	};

	inline MainArgs::MainArgs(int argc, char **argv)
	{
		for (int i = 0; i < argc; ++i)
		{
			args.emplace_back(argv[i]);
		}
	}

	inline size_t MainArgs::size() const
	{
		return args.size();
	}

	inline auto MainArgs::begin() const
	{
		return args.begin();
	}

	inline auto MainArgs::end() const
	{
		return args.end();
	}
}
