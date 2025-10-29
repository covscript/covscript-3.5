#pragma once
#include <covscript/common/platform.hpp>
#include <covscript/types/types.hpp>
#include <forward_list>
#include <vector>
#include <memory>

namespace cs
{

	// Note: each fiber/thread should have its own memory manager instance.
	class memory_manager final
	{
		struct heap_record
		{
#ifdef COVSCRIPT_DEBUG
			string name = "<Unknown>";
#endif
			std::size_t reachable_count = 0;
			var data;
		};

		std::forward_list<heap_record> m_heap;
		stack<var> m_stack;

		struct domain final
		{
#ifdef COVSCRIPT_DEBUG
			string name = "<Temporary>";
#endif
			std::size_t stack_start = 0;
			map_t<string, std::size_t> slot_map;
			std::shared_ptr<bool> is_active = std::make_shared<bool>(true);
			domain() = default;
			domain(std::size_t stack_size) : stack_start(stack_size) {}
			~domain() { *is_active = false; }
		};

		stack<domain> m_domain_stack;

	   public:
		// For visiting stack memory
		class visitor final
		{
			friend class memory_manager;
			std::shared_ptr<bool> is_active;
			const memory_manager *m_manager = nullptr;
			std::size_t stack_idx = 0;
			visitor() = default;

		   public:
			string name;
		};

		memory_manager(string_borrower name = "<Global>")
		{
			push_domain(name);
		}

		void push_domain(string_borrower name = "<Temporary>")
		{
			m_domain_stack.push(m_stack.size());
#ifdef COVSCRIPT_DEBUG
			m_domain_stack.top().name = name.view();
#endif
		}

		void pop_domian()
		{
			m_domain_stack.pop_no_return();
		}

		visitor add_var(const string &name, const var &value, bool override = false)
		{
			domain &d = m_domain_stack.top();
			auto it = d.slot_map.find(name);
			if (it != d.slot_map.end())
			{
				if (override)
				{
					m_stack[it->second] = value;
					visitor v;
					v.name = name;
					v.is_active = d.is_active;
					v.m_manager = this;
					v.stack_idx = it->second;
					return v;
				}
				else
					throw runtime_error("Variable \"" + name + "\" already defined in current scope.");
			}
			else
			{
				m_stack.push(value);
				std::size_t idx = m_stack.size() - 1;
				d.slot_map.emplace(name, idx);
				visitor v;
				v.name = name;
				v.is_active = d.is_active;
				v.m_manager = this;
				v.stack_idx = idx;
				return v;
			}
		}

        // TODO: fix
        void declare_var(visitor &v, const var &value, bool override = false)
		{
			domain &d = m_domain_stack.top();
			auto it = d.slot_map.find(v.name);
			if (it != d.slot_map.end())
			{
				if (override)
				{
					m_stack[it->second] = value;
					visitor v;
					v.name = name;
					v.is_active = d.is_active;
					v.m_manager = this;
					v.stack_idx = it->second;
					return v;
				}
				else
					throw runtime_error("Variable \"" + name + "\" already defined in current scope.");
			}
			else
			{
				m_stack.push(value);
				std::size_t idx = m_stack.size() - 1;
				d.slot_map.emplace(name, idx);
				visitor v;
				v.name = name;
				v.is_active = d.is_active;
				v.m_manager = this;
				v.stack_idx = idx;
				return v;
			}
		}

		var &access(visitor &v)
		{
			if (v.m_manager == this && *v.is_active)
				return m_stack[v.stack_idx];
			else
			{
				for (auto &d : m_domain_stack)
				{
					auto it = d.slot_map.find(v.name);
					if (it != d.slot_map.end())
					{
						v.is_active = d.is_active;
						v.m_manager = this;
						v.stack_idx = it->second;
						return m_stack[v.stack_idx];
					}
				}
				throw runtime_error("Use of undefined variable \"" + v.name + "\".");
			}
		}

		var *access_opt(visitor &v) noexcept
		{
			if (v.m_manager == this && *v.is_active)
				return &m_stack[v.stack_idx];
			else
			{
				for (auto &d : m_domain_stack)
				{
					auto it = d.slot_map.find(v.name);
					if (it != d.slot_map.end())
					{
						v.is_active = d.is_active;
						v.m_manager = this;
						v.stack_idx = it->second;
						return &m_stack[v.stack_idx];
					}
				}
				return nullptr;
			}
		}
	};
} // namespace cs