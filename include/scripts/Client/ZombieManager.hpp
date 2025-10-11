/**
 * @file Resource/Scripts/Client.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

 #pragma once

#include "Gameplay/ObjectManager.hpp"
#include "Scripts/Client/Zombie.hpp"

namespace tudov
{
	class ZombieManager : public ObjectManager<Zombie>
	{
	};
} // namespace tudov