/**
 * @file program/Window.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <exception>
#include <sstream>

namespace tudov
{
	class LuaBindings;

	using ObjectID = std::uint32_t;

	template <typename TObject>
	class ObjectManager
	{
		friend LuaBindings;

	  public:
		void Spawn()
		{
			//
		}

		ObjectID Despawn()
		{
			//
		}

		std::stringbuf Serialize()
		{
			//
		}

		void Deserialize()
		{
			//
		}

	  private:
		void LuaSerialize()
		{
			try
			{
				return Serialize();
			}
			catch (std::exception &e)
			{
				// TODO
			}
		}
	};
} // namespace tudov
