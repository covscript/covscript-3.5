#pragma once
#include <csvm/common/platform.hpp>
#include <csvm/types/types.hpp>
#include <vector>
#include <memory>
#include <list>

#ifndef CSVM_GC_THRESHOLD
#define CSVM_GC_THRESHOLD 1024
#endif

namespace csvm
{
	// Note: each fiber/thread should have its own memory manager instance.
	class memory_manager final
	{
	   public:
		// For visiting stack memory
		class stack_visitor final
		{
			friend class memory_manager;
			std::size_t stack_idx = 0;
			std::shared_ptr<bool> is_active;
			const memory_manager *manager = nullptr;

		   public:
			string name;
			stack_visitor() = default;
			stack_visitor(string_view str) : name(str) {}
		};

		// For visiting heap memory
		class heap_pointer final
		{
			friend class memory_manager;
#ifdef CSVM_DEBUG
			string name = "<Unknown>";
#endif
			mutable std::size_t reachable_count = 0;

			heap_pointer() = default;
			heap_pointer(var val) : data(std::move(val)) {}

		   public:
			var data;

			void mark_reachable() const noexcept
			{
				++reachable_count;
			}
		};

		class domain final
		{
			friend class memory_manager;
#ifdef CSVM_DEBUG
			string name = "<Temporary>";
#endif
			bool is_temp = true;
			std::size_t stack_start = 0;
			map_t<string, std::size_t> slot_map;
			std::shared_ptr<bool> is_active = std::make_shared<bool>(true);

			domain() = default;

		   public:
			~domain() { *is_active = false; }
		};

	   private:
		stack<std::size_t> m_domain_stack;
		stack<var> m_stack;

		std::list<heap_pointer> m_heap;
		std::size_t m_last_heap_size = 0;
		std::size_t m_gc_threshold = 0;

		inline domain &get_top_domain()
		{
			return m_stack[m_domain_stack.top()].val<domain>();
		}

	   public:
		memory_manager(string_view name = "<Global>",
		               std::size_t stack_size = CSVM_STACK_PRESERVE,
		               std::size_t gc_threshold = CSVM_GC_THRESHOLD) : m_gc_threshold(gc_threshold)
		{
			m_stack.resize(stack_size);
			enter_domain(name);
		}

		void enter_domain(string_view name = "<Temporary>", bool declare = false)
		{
			std::size_t stack_start = m_stack.size();
			if (declare)
				get_top_domain().slot_map.emplace(name, stack_start);
			m_stack.push(var::make<domain>());
			m_domain_stack.push(stack_start);
			domain &d = get_top_domain();
			d.is_temp = !declare;
			d.stack_start = stack_start;
#ifdef CSVM_DEBUG
			d.name = name.view();
#endif
		}

		void leave_domain(bool force_clear = false)
		{
			domain &d = get_top_domain();
			if (d.is_temp || force_clear)
			{
				std::size_t stack_start = m_domain_stack.pop();
				while (m_stack.size() > stack_start)
					m_stack.pop_no_return();
			}
			else
				m_domain_stack.pop_no_return();
		}

		void declare_var(const string &name, const var &value, bool override = false)
		{
			domain &d = get_top_domain();
			auto it = d.slot_map.find(name);
			if (it != d.slot_map.end())
			{
				if (override)
					m_stack[it->second] = value;
				else
					throw runtime_error("Variable \"" + name + "\" already defined in current scope.");
			}
			else
			{
				d.slot_map.emplace(name, m_stack.size());
				m_stack.push(value);
			}
		}

		var &access(stack_visitor &v)
		{
			if (v.manager == this && *v.is_active)
				return m_stack[v.stack_idx];
			else
			{
				for (auto &idx : m_domain_stack)
				{
					domain &d = m_stack[idx].val<domain>();
					auto it = d.slot_map.find(v.name);
					if (it != d.slot_map.end())
					{
						v.is_active = d.is_active;
						v.manager = this;
						v.stack_idx = it->second;
						return m_stack[v.stack_idx];
					}
				}
				throw runtime_error("Use of undefined variable \"" + v.name + "\".");
			}
		}

		var *access_opt(stack_visitor &v) noexcept
		{
			if (v.manager == this && *v.is_active)
				return &m_stack[v.stack_idx];
			else
			{
				for (auto &idx : m_domain_stack)
				{
					domain &d = m_stack[idx].val<domain>();
					auto it = d.slot_map.find(v.name);
					if (it != d.slot_map.end())
					{
						v.is_active = d.is_active;
						v.manager = this;
						v.stack_idx = it->second;
						return &m_stack[v.stack_idx];
					}
				}
				return nullptr;
			}
		}

		heap_pointer *gcnew()
		{
			m_heap.emplace_front();
			return &m_heap.front();
		}

		template <typename T, typename... ArgsT>
		heap_pointer *gcnew(ArgsT &&...args)
		{
			m_heap.emplace_front(var::make<T>(std::forward<ArgsT>(args)...));
			return &m_heap.front();
		}

		std::size_t gc(bool force = false);
	};
} // namespace csvm

template <>
void cs_impl::mark_reachable<cs::memory_manager::heap_pointer>(const cs::memory_manager::heap_pointer &ptr)
{
	ptr.mark_reachable();
}