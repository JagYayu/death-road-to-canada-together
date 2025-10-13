/**
 * @file audio/AudioPlayer.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "Math/Vector.hpp"
#include "Program/EngineComponent.hpp"
#include "System/Log.hpp"

#include "sol/table.hpp"

#include <memory>

struct ALCdevice;
struct ALCcontext;

namespace tudov
{
	class Audio;
	class AudioSource;
	class LuaBindings;

	struct PlayAudioArgs
	{
		Audio *audio;
		Vector3F *position;

		bool loop = false;
		std::float_t pitch = 1.0f;
		std::float_t gain = 1.0f;
		std::float_t rolloffFactor = -1.0f;
		std::float_t referenceDistance = -1.0f;
		std::float_t maxDistance = -1.0f;
	};

	struct IAudioPlayer
	{
		std::shared_ptr<AudioSource> Play(const PlayAudioArgs &args);
	};

	class AudioPlayer final : public IAudioPlayer, public IContextProvider, ILogProvider
	{
		friend LuaBindings;

	  private:
		Context &_context;
		ALCdevice *_alDevice;
		ALCcontext *_alContext;

	  public:
		AudioPlayer(Context &context) noexcept;
		~AudioPlayer() noexcept;

		Context &GetContext() noexcept override;
		Log &GetLog() noexcept override;

		std::shared_ptr<AudioSource> Play(const PlayAudioArgs &args);

		void LuaPlay(sol::table args) noexcept;

		void Update() noexcept;
	};
} // namespace tudov
