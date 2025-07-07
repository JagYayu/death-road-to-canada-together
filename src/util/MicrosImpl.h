#pragma once

#define TUDOV_GEN_END static_assert(true)

#define TUDOV_GEN_GETTER_REF(Return, Function, Member, Suffix) \
	Return &Function() Suffix                                  \
	{                                                          \
		return Member;                                         \
	}                                                          \
	const Return &Function() const Suffix                      \
	{                                                          \
		return Member;                                         \
	}                                                          \
	TUDOV_GEN_END

#define TUDOV_GEN_GETTER_SMART_PTR(SmartPointer, Return, Function, Member, Suffix) \
	SmartPointer<Return> Function() Suffix                                         \
	{                                                                              \
		return SmartPointer<Return>(Member);                                       \
	}                                                                              \
	SmartPointer<const Return> Function() const Suffix                             \
	{                                                                              \
		return SmartPointer<Return>(Member);                                       \
	}                                                                              \
	TUDOV_GEN_END
