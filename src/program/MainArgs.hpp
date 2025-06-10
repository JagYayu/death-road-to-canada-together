#pragma once

#include "util/Defs.h"

namespace tudov
{
	class MainArgs
	{
	  private:
		Vector<String> args;

	  public:
		explicit MainArgs(int argc, char **argv);

		size_t size() const;

		const String &operator[](size_t i) const
		{
			return args[i];
		}

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
} // namespace tudov
