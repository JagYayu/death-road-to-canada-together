#ifdef TE_MICROS_IMPL_HPP
#error "MicrosImpl.hpp" file should be include in ".cpp" file without duplication.
#endif
#define TE_MICROS_IMPL_HPP

#define TE_GEN_END static_assert(true)

#define TE_GEN_GETTER_REF(Return, Function, Member, Suffix) \
	Return &Function() Suffix                               \
	{                                                       \
		return Member;                                      \
	}                                                       \
	const Return &Function() const Suffix                   \
	{                                                       \
		return Member;                                      \
	}                                                       \
	TE_GEN_END

#define TE_GEN_GETTER_SMART_PTR(SmartPointer, Return, Function, Member, Suffix) \
	SmartPointer<Return> Function() Suffix                                      \
	{                                                                           \
		return SmartPointer<Return>(Member);                                    \
	}                                                                           \
	SmartPointer<const Return> Function() const Suffix                          \
	{                                                                           \
		return SmartPointer<Return>(Member);                                    \
	}                                                                           \
	TE_GEN_END
