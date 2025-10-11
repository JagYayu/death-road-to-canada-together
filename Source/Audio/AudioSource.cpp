/**
 * @file audio/AudioSource.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Audio/AudioSource.hpp"
#include "Audio/AudioPlayer.hpp"

using namespace tudov;

IAudioPlayer *AudioSource::GetIAudioPlayer() noexcept
{
	return _audioPlayer;
}

void AudioSource::SetPosition(std::float_t) noexcept
{
	//
}

void AudioSource::SetDirection() noexcept
{
	//
}

void AudioSource::MarkExpired() noexcept
{
	_audioPlayer = nullptr;
}
