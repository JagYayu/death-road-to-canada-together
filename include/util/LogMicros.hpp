#pragma once

#include "util/Log.hpp" // IWYU pragma: keep

#define TE_G_LOG(Name, Verb, Format, ...)                       \
	if (auto &&log = ::tudov::Log::Get(Name); log->Can##Verb()) \
	log->Verb(Format, __VA_ARGS__)

#define TE_G_TRACE(Name, Format, ...) TE_G_LOG(Name, Trace, Format, __VA_ARGS__)
#define TE_G_DEBUG(Name, Format, ...) TE_G_LOG(Name, Debug, Format, __VA_ARGS__)
#define TE_G_INFO(Name, Format, ...)  TE_G_LOG(Name, Info, Format, __VA_ARGS__)
#define TE_G_WARN(Name, Format, ...)  TE_G_LOG(Name, Warn, Format, __VA_ARGS__)
#define TE_G_ERROR(Name, Format, ...) TE_G_LOG(Name, Error, Format, __VA_ARGS__)
#define TE_G_FATAL(Name, Format, ...) TE_G_LOG(Name, Fatal, Format, __VA_ARGS__)

#define TE_LOG(Verb, Format, ...) \
	if (this->Can##Verb())        \
	this->Verb(Format, __VA_ARGS__)

#define TE_TRACE(Format, ...) TE_LOG(Trace, Format, __VA_ARGS__)
#define TE_DEBUG(Format, ...) TE_LOG(Debug, Format, __VA_ARGS__)
#define TE_INFO(Format, ...)  TE_LOG(Info, Format, __VA_ARGS__)
#define TE_WARN(Format, ...)  TE_LOG(Warn, Format, __VA_ARGS__)
#define TE_ERROR(Format, ...) TE_LOG(Error, Format, __VA_ARGS__)
#define TE_FATAL(Format, ...) TE_LOG(Fatal, Format, __VA_ARGS__)
