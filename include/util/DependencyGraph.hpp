/**
 * @file util/DependencyGraph.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include <algorithm>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename TNode, typename TLinks = std::unordered_map<TNode, std::unordered_set<TNode>>>
class DependencyGraph
{
  private:
	static std::optional<std::vector<TNode>> Traverse(const TLinks &links, const TNode &beginNode) noexcept
	{
		std::optional<std::vector<TNode>> result{std::vector<TNode>()};
		std::unordered_map<TNode, bool> visited;
		if (TraverseImpl(links, beginNode, result, visited).has_value())
		{
			return std::nullopt;
		}

		auto &&res = result.value();
		if (!res.empty())
		{
			res.pop_back();
		}
		std::ranges::reverse(res);
		return res;
	}

	static std::optional<TNode> TraverseImpl(const TLinks &links, const TNode &node, std::optional<std::vector<TNode>> &result, std::unordered_map<TNode, bool> &visited) noexcept
	{
		{
			auto it = visited.find(node);
			if (it != visited.end())
			{
				return it->second ? std::optional<TNode>(node) : std::nullopt;
			}
		}

		if (auto it = links.find(node); it != links.end())
		{
			visited[node] = true;
			for (const auto &next : it->second)
			{
				if (auto cycle = TraverseImpl(links, next, result, visited); cycle.has_value())
				{
					return cycle;
				}
			}
		}

		visited[node] = false;
		if (result.has_value())
		{
			result.value().push_back(node);
		}
		return std::nullopt;
	}

	static void BreakAllLinks(TLinks &links, TLinks &backLinks, const TNode &node) noexcept
	{
		auto it = links.find(node);
		if (it == links.end())
		{
			return;
		}

		std::vector<TNode> targets{it->second.begin(), it->second.end()};
		for (const auto &target : targets)
		{
			links[node].erase(target);
			backLinks[target].erase(node);
			if (backLinks[target].empty())
			{
				backLinks.erase(target);
			}
		}
		links.erase(node);
	}

  private:
	TLinks _forwardLinks;
	TLinks _backwardLinks;

  public:
	using TLink = std::vector<std::pair<TNode, TNode>>;

	explicit DependencyGraph() noexcept = default;
	explicit DependencyGraph(const DependencyGraph &) noexcept = default;
	explicit DependencyGraph(DependencyGraph &&) noexcept = default;
	DependencyGraph &operator=(const DependencyGraph &) noexcept = default;
	DependencyGraph &operator=(DependencyGraph &&) noexcept = default;
	~DependencyGraph() noexcept = default;

	void Clear() noexcept
	{
		_forwardLinks.clear();
		_backwardLinks.clear();
	}

	void AddLink(const TNode &from, const TNode &to) noexcept
	{
		_forwardLinks[from].insert(to);
		_backwardLinks[to].insert(from);
	}

	void RemoveLink(const TNode &from, const TNode &to) noexcept
	{
		auto it = _forwardLinks.find(from);
		if (it != _forwardLinks.end())
		{
			it->second.erase(to);
			if (it->second.empty())
			{
				_forwardLinks.erase(it);
			}
		}

		auto &&itBack = _backwardLinks.find(to);
		if (itBack != _backwardLinks.end())
		{
			itBack->second.erase(from);
			if (itBack->second.empty())
			{
				_backwardLinks.erase(itBack);
			}
		}
	}

	std::optional<std::vector<TNode>> GetForwardTraversal(const TNode &startNode) const noexcept
	{
		return Traverse(_forwardLinks, startNode);
	}

	std::optional<std::vector<TNode>> GetBackwardTraversal(const TNode &startNode) const noexcept
	{
		return Traverse(_backwardLinks, startNode);
	}

	bool CheckConnection(const TNode &from, const TNode &to) const noexcept
	{
		std::unordered_map<TNode, bool> visited;
		std::optional<std::vector<TNode>> result = std::nullopt;
		TraverseImpl(_forwardLinks, from, result, visited);
		return visited.contains(to);
	}

	void Unlink(const TNode &node) noexcept
	{
		BreakAllLinks(_forwardLinks, _backwardLinks, node);
		BreakAllLinks(_backwardLinks, _forwardLinks, node);
	}

	void UnlinkOutgoing(const TNode &node) noexcept
	{
		BreakAllLinks(_forwardLinks, _backwardLinks, node);
	}

	void UnlinkIncoming(const TNode &node)
	{
		BreakAllLinks(_backwardLinks, _forwardLinks, node);
	}

	std::pair<TLink, TLink> GetAllLinks() const noexcept
	{
		TLink forwardLinks{};
		TLink backwardLinks{};

		for (const auto &&[from, targets] : _forwardLinks)
		{
			for (const auto &to : targets)
			{
				forwardLinks.emplace_back(from, to);
			}
		}

		for (const auto &&[to, sources] : _backwardLinks)
		{
			for (const auto &from : sources)
			{
				backwardLinks.emplace_back(from, to);
			}
		}

		auto &&comp = [](const auto &l, const auto &r) constexpr
		{
			return std::tie(l.first, l.second) < std::tie(r.first, r.second);
		};
		std::ranges::sort(forwardLinks, comp);
		std::ranges::sort(backwardLinks, comp);

		return {forwardLinks, backwardLinks};
	}
};
