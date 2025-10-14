/**
 * @file program/Memory.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "Util/Micros.hpp"

#include "mimalloc.h"

#include <cstddef>

namespace tudov
{
	class Memory
	{
	  public:
		TE_FORCEINLINE static std::size_t GetUsage() noexcept
		{
			return 0;
		}

		TE_FORCEINLINE static void *LuaAlloc(void *ud, void *ptr, std::size_t osize, std::size_t nsize) noexcept
		{
			if (ud != nullptr)
			{
				if (nsize != 0)
				{
					return mi_realloc(ptr, nsize);
				}
				else
				{
					mi_free(ptr);
					return nullptr;
				}
			}
			else
			{
				return static_cast<Memory *>(ud)->LuaHeapAlloc(ptr, osize, nsize);
			}
		}

		TE_FORCEINLINE static void *Malloc(std::size_t size) noexcept
		{
			return mi_malloc(size);
		}

		TE_FORCEINLINE static void *Calloc(std::size_t count, std::size_t size) noexcept
		{
			return mi_calloc(count, size);
		}

		TE_FORCEINLINE static void *Realloc(void *p, std::size_t newsize) noexcept
		{
			return mi_realloc(p, newsize);
		}

		TE_FORCEINLINE static void Free(void *p) noexcept
		{
			mi_free(p);
		}

		TE_FORCEINLINE static void Free(void *p, size_t size) noexcept
		{
			mi_free_size(p, size);
		}

	  public:
		TE_FORCEINLINE explicit Memory() noexcept
		{
			_heap = mi_heap_new();
		}

		TE_FORCEINLINE ~Memory() noexcept
		{
			mi_heap_delete(_heap);
		}

		TE_FORCEINLINE std::size_t GetHeapUsage() noexcept
		{
			return 0;
		}

		TE_FORCEINLINE void *HeapMalloc(std::size_t size) noexcept
		{
			return mi_heap_malloc(_heap, size);
		}

		TE_FORCEINLINE void *HeapCalloc(std::size_t count, std::size_t size) noexcept
		{
			return mi_heap_calloc(_heap, count, size);
		}

		TE_FORCEINLINE void *HeapRealloc(void *p, std::size_t newsize) noexcept
		{
			return mi_heap_realloc(_heap, p, newsize);
		}

		TE_FORCEINLINE void HeapFree(void *p) noexcept
		{
			Memory::Free(p);
		}

	  private:
		TE_FORCEINLINE void *LuaHeapAlloc(void *ptr, std::size_t osize, std::size_t nsize) noexcept
		{
			if (nsize == 0)
			{
				Memory::Free(ptr);
				return nullptr;
			}
			else
			{
				return mi_heap_realloc(_heap, ptr, nsize);
			}
		}

	  private:
		mi_heap_t *_heap;
	};

	template <typename T, Memory *Heap>
	struct Allocator
	{
		TE_FORCEINLINE explicit Allocator() noexcept = default;

		template <typename T1, Memory *Heap2>
		TE_FORCEINLINE Allocator(const Allocator<T1, Heap2> &) noexcept
		{
		}

		TE_FORCEINLINE ~Allocator() noexcept = default;

		TE_FORCEINLINE T *allocate(std::size_t n)
		{
			if constexpr (Heap == nullptr)
			{
				return static_cast<T *>(Memory::Malloc(n * sizeof(T)));
			}
			else
			{
				return static_cast<T *>(Heap->Malloc(n * sizeof(T)));
			}
		}

		TE_FORCEINLINE void deallocate(T *p, std::size_t size) noexcept
		{
			Heap->Free(p, size);
		}
	};
} // namespace tudov
