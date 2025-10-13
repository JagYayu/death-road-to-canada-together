/**
 * @file util/MicrosImpl.hpp
 * @author JagYayu
 * @brief Micros for project implementation files.
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#define TE_MICRO_END static_assert(true)

#define TE_GEN_GETTER_REF(Return, Function, Member, Suffix) \
	Return &Function() Suffix                               \
	{                                                       \
		return Member;                                      \
	}                                                       \
	const Return &Function() const Suffix                   \
	{                                                       \
		return Member;                                      \
	}                                                       \
	TE_MICRO_END

#define TE_GEN_GETTER_SMART_PTR(SmartPointer, Return, Function, Member, Suffix) \
	SmartPointer<Return> Function() Suffix                                      \
	{                                                                           \
		return SmartPointer<Return>(Member);                                    \
	}                                                                           \
	SmartPointer<const Return> Function() const Suffix                          \
	{                                                                           \
		return SmartPointer<Return>(Member);                                    \
	}                                                                           \
	TE_MICRO_END

#pragma region LuaBindings

#define TE_LB_ENUM(Class, ...) lua.new_enum<Class>( \
	#Class,                                         \
	__VA_ARGS__)

#define TE_LB_USERTYPE(Class, ...) lua.new_usertype<Class>( \
	#Class,                                                 \
	"getTypeID", []() { return TypeID{&typeid(Class)}; },  \
	__VA_ARGS__)

#define TE_LB_CLASS(Class, ...) lua.create_named_table( \
	("" #Class ""),                                     \
	__VA_ARGS__)

#pragma endregion
