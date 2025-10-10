/**
 * @file resource/AudioResources.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Resources.hpp"
#include "audio/Audio.hpp"

namespace tudov
{
	template <>
	inline Resources<Audio>::Resources() noexcept
	    : _log(Log::Get(TE_NAMEOF(AudioResources)))
	{
	}

	class LuaBindings;
	class ScriptEngine;

	class AudioResources final : public Resources<Audio>
	{
		friend LuaBindings;

	  public:
		explicit AudioResources() noexcept;
		explicit AudioResources(const AudioResources &) noexcept = delete;
		explicit AudioResources(AudioResources &&) noexcept = delete;
		AudioResources &operator=(const AudioResources &) noexcept = delete;
		AudioResources &operator=(AudioResources &&) noexcept = delete;
		~AudioResources() noexcept = default;
	};
} // namespace tudov
