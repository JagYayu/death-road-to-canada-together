/**
 * @file audio/AudioPlayer.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Audio/AudioPlayer.hpp"
#include "System/LogMicros.hpp"

#include <AL/al.h>
#include <AL/alc.h>

using namespace tudov;

AudioPlayer::AudioPlayer(Context &context) noexcept
    : _context(context)
{
	_alDevice = alcOpenDevice(nullptr);
	if (_alDevice == nullptr)
	{
		TE_ERROR("Failed to open OpenAL device");
	}

	_alContext = alcCreateContext(_alDevice, nullptr);
	if (!alcMakeContextCurrent(_alContext))
	{
		TE_WARN("Failed to set OpenAL context");
	}

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	ALfloat ori[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
	alListenerfv(AL_ORIENTATION, ori);
}

AudioPlayer::~AudioPlayer() noexcept
{
}

Context &AudioPlayer::GetContext() noexcept
{
	return _context;
}

Log &AudioPlayer::GetLog() noexcept
{
	return *Log::Get(TE_NAMEOF(AudioPlayer));
}

std::shared_ptr<AudioSource> AudioPlayer::Play(const PlayAudioArgs &args)
{
	// TODO
	return nullptr;
}
