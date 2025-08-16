/**
 * @file program/CrashReporter.cpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#include "program/CrashReporter.hpp"

#if _WIN32

using namespace tudov;

void CrashReporter::InitializeCrashReporter() noexcept
{
}

#else

using namespace tudov;

void CrashReporter::InitializeCrashReporter() noexcept
{
}

#endif
