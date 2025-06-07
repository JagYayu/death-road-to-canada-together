#pragma once

#include "util/Micros.h"

#include <string>
	

namespace tudov
{
	INTERFACE IConfig
	{
		void Save() = 0;
		void Load() = 0;

		virtual std::string GetWindowTitle() = 0;
		virtual uint32_t GetWindowWidth() = 0;
		virtual uint32_t GetWindowHeight() = 0;

		virtual void SetWindowTitle(const std::string&) = 0;
		virtual void SetWindowWidth(uint32_t) = 0;
		virtual void SetWindowHeight(uint32_t) = 0;
	};
}
