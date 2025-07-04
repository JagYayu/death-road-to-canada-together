#pragma once

#include "bgfx/bgfx.h"
#include <string>

namespace tudov
{
	class Shader
	{
	  public:
		Shader(const std::string &name);
		~Shader();

		bool load(const std::string &vsPath, const std::string &fsPath);
		void unload();

		bgfx::ProgramHandle handle() const;

		bool isValid() const;
		const std::string &getName() const;

	  private:
		std::string name;
		bgfx::ShaderHandle vs = BGFX_INVALID_HANDLE;
		bgfx::ShaderHandle fs = BGFX_INVALID_HANDLE;
		bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
	};

} // namespace tudov
