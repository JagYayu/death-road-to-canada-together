/**
 * @file system/LogMicros.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "System/Log.hpp" // IWYU pragma: keep

#pragma region Global Logging

#define TE_G_LOG(Name, Verb, Format, ...)                       \
	if (auto &&log = ::tudov::Log::Get(Name); log->Can##Verb()) \
	log->Verb(Format __VA_OPT__(, __VA_ARGS__))

#define TE_G_TRACE(Name, Format, ...) TE_G_LOG(Name, Trace, Format, __VA_ARGS__)
#define TE_G_DEBUG(Name, Format, ...) TE_G_LOG(Name, Debug, Format, __VA_ARGS__)
#define TE_G_INFO(Name, Format, ...)  TE_G_LOG(Name, Info, Format, __VA_ARGS__)
#define TE_G_WARN(Name, Format, ...)  TE_G_LOG(Name, Warn, Format, __VA_ARGS__)
#define TE_G_ERROR(Name, Format, ...) TE_G_LOG(Name, Error, Format, __VA_ARGS__)
#define TE_G_FATAL(Name, Format, ...) ::tudov::Log::Get(Name)->Fatal(Format __VA_OPT__(, __VA_ARGS__))

#pragma endregion

#pragma region Local Logging

#define TE_L_LOG(Name, Verb, Format, ...) \
	if (TE_L_log->Can##Verb())            \
	TE_L_log->Verb(Format __VA_OPT__(, __VA_ARGS__))

#define TE_L_TRACE(Format, ...) TE_L_LOG(Name, Trace, Format, __VA_ARGS__)
#define TE_L_DEBUG(Format, ...) TE_L_LOG(Name, Debug, Format, __VA_ARGS__)
#define TE_L_INFO(Format, ...)  TE_L_LOG(Name, Info, Format, __VA_ARGS__)
#define TE_L_WARN(Format, ...)  TE_L_LOG(Name, Warn, Format, __VA_ARGS__)
#define TE_L_ERROR(Format, ...) TE_L_LOG(Name, Error, Format, __VA_ARGS__)
#define TE_L_FATAL(Format, ...) TE_L_LOG(Name, Fatal, Format, __VA_ARGS__)

#pragma endregion

#pragma region Class Logging

#define TE_LOG(Verb, Format, ...) \
	if (this->Can##Verb())        \
	this->Verb(Format __VA_OPT__(, __VA_ARGS__))

#define TE_TRACE(Format, ...) TE_LOG(Trace, Format, __VA_ARGS__)
#define TE_DEBUG(Format, ...) TE_LOG(Debug, Format, __VA_ARGS__)
#define TE_INFO(Format, ...)  TE_LOG(Info, Format, __VA_ARGS__)
#define TE_WARN(Format, ...)  TE_LOG(Warn, Format, __VA_ARGS__)
#define TE_ERROR(Format, ...) TE_LOG(Error, Format, __VA_ARGS__)
#define TE_FATAL(Format, ...) TE_LOG(Fatal, Format, __VA_ARGS__)

#pragma endregion
