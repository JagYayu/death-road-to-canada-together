/**
 * @file data/Hierarchy.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "HierarchyElement.hpp"
#include "HierarchyIterationResult.hpp"
#include "util/EnumFlag.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>

namespace tudov
{
	/**
	 * Interface Class
	 */
	template <typename TData>
	struct IHierarchy
	{
		using Path = std::filesystem::path;

		using IterationCallback = std::function<EHierarchyIterationResult(const Path &path, const Path &directory, void *args)>;

		/**
		 * Count numbers of specific elements with type.
		 */
		virtual std::size_t Count(const Path &directory, EHierarchyElement element = EHierarchyElement::All)
		{
			std::size_t count = 0;

			Foreach(directory, [this, &count, element](const Path &path, const Path &, void *) -> EHierarchyIterationResult
			{
				if (EnumFlag::HasAny(element, Check(path)))
				{
					++count;
				}

				return EHierarchyIterationResult::Continue;
			});

			return count;
		}

		/**
		 * Count numbers of specific elements with type, recursed.
		 */
		virtual std::size_t CountRecursed(const Path &directory, EHierarchyElement element = EHierarchyElement::All)
		{
			std::size_t count = 0;

			ForeachRecursed(directory, [this, &count, element](const Path &path, const Path &, void *) -> EHierarchyIterationResult
			{
				if (EnumFlag::HasAny(element, Check(path)))
				{
					++count;
				}

				return EHierarchyIterationResult::Continue;
			});

			return count;
		}

		/**
		 * Check the element from given path.
		 */
		virtual EHierarchyElement Check(const Path &path) noexcept = 0;

		/**
		 * Check if the element from given path is data.
		 */
		virtual bool IsData(const Path &path) noexcept
		{
			return EnumFlag::HasAny(Check(path), EHierarchyElement::Data);
		}

		/**
		 * Check if the element from given path is directory.
		 */
		virtual bool IsDirectory(const Path &path) noexcept
		{
			return EnumFlag::HasAny(Check(path), EHierarchyElement::Directory);
		}

		/**
		 * Check if the element from given path not exists.
		 */
		virtual bool IsNone(const Path &path) noexcept
		{
			return Check(path) == EHierarchyElement::None;
		}

		/**
		 * Get the data element from given path.
		 */
		virtual TData Get(const Path &dataPath) = 0;

		virtual EHierarchyIterationResult Foreach(const Path &directory, const IterationCallback &callback, void *callbackArgs = nullptr) = 0;

		virtual EHierarchyIterationResult ForeachRecursed(const Path &directory, const IterationCallback &callback, std::uint32_t maxDepth = -1, void *callbackArgs = nullptr)
		{
			if (maxDepth == 0) [[unlikely]]
			{
				return EHierarchyIterationResult::Success;
			}

			auto &&callbackRecursed = [this, &callback, maxDepth](const std::filesystem::path &path, const std::filesystem::path &directory, void *args) -> EHierarchyIterationResult
			{
				if (!path.has_extension())
				{
					EHierarchyIterationResult result = ForeachRecursed(directory / path, callback, maxDepth - 1, args);
					switch (result)
					{
					case EHierarchyIterationResult::Continue:
						break;
					default:
						return result;
					}
				}

				return callback(path, directory, args);
			};

			return Foreach(directory, callbackRecursed, callbackArgs);
		}

		const TData Get(const Path &path) const
		{
			return const_cast<IHierarchy<IHierarchy>>(this).Get(path);
		}
	};
} // namespace tudov
