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
		explicit MainArgs();
		explicit MainArgs(int argc, char **argv);

		size_t Size() const;

		const std::string &operator[](size_t i) const;

		auto begin() const;
		auto end() const;
	};

	inline MainArgs::MainArgs()
	    : MainArgs(0, nullptr)
	{
	}

	inline MainArgs::MainArgs(int argc, char **argv)
	{
		for (int i = 0; i < argc; ++i)
		{
			args.emplace_back(argv[i]);
		}
	}

	inline size_t MainArgs::Size() const
	{
		return args.size();
	}

	inline const std::string &MainArgs::operator[](size_t i) const
	{
		return args[i];
	}

	inline auto MainArgs::begin() const
	{
		return args.begin();
	}

	inline auto MainArgs::end() const
	{
		return args.end();
	}
} // namespace tudov
