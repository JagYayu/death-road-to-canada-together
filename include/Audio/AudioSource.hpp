/**
 * @file audio/AudioHandle.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "util/Micros.hpp"

#include <cmath>

namespace tudov
{
	struct IAudioPlayer;
	class AudioPlayer;

	struct IAudioSource
	{
		friend IAudioPlayer;

		virtual IAudioPlayer *GetIAudioPlayer() noexcept = 0;
		virtual void SetPosition(std::float_t) noexcept = 0;
		virtual void SetDirection() noexcept = 0;

	  private:
		virtual void MarkExpired() noexcept = 0;

	  public:
		TE_FORCEINLINE bool IsExpired() noexcept
		{
			return GetIAudioPlayer() == nullptr;
		}
	};

	class AudioSource final : IAudioSource
	{
	  private:
		AudioPlayer *_audioPlayer;

	  public:
		explicit AudioSource(AudioPlayer *audioPlayer) noexcept;

		IAudioPlayer *GetIAudioPlayer() noexcept override;
		void SetPosition(std::float_t) noexcept override;
		void SetDirection() noexcept override;
		void MarkExpired() noexcept override;

		AudioPlayer *GetAudioPlayer() noexcept;
	};
} // namespace tudov
