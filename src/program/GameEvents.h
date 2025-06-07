#pragma once

namespace tudov
{
	class GameEvents
	{
	private:
	public:
		void Poll()
		{
			// 这里以 SDL 为例，实际应根据具体窗口系统实现
			// SDL_Event sdlEvent;
			// while (SDL_PollEvent(&sdlEvent))
			// {
			// 	Event event;
			// 	switch (sdlEvent.type)
			// 	{
			// 	case SDL_QUIT:
			// 		event.type = EventType::Quit;
			// 		break;
			// 	case SDL_KEYDOWN:
			// 		event.type = EventType::KeyDown;
			// 		break;
			// 	case SDL_KEYUP:
			// 		event.type = EventType::KeyUp;
			// 		break;
			// 	case SDL_MOUSEMOTION:
			// 		event.type = EventType::MouseMove;
			// 		break;
			// 	// 添加其他事件类型的处理
			// 	default:
			// 		continue;
			// 	}

			// 	for (auto &listener : listeners)
			// 	{
			// 		listener->OnEvent(event);
			// 	}
			// }
		}
	};
}
