#pragma once
#include <vector>

#ifndef COVSCRIPT_STACK_PRESERVE
#define COVSCRIPT_STACK_PRESERVE 64
#endif

namespace cs {
	template<typename T>
	class stack final {
		std::vector<T> m_impl;
	public:
		using iterator = typename std::vector<T>::reverse_iterator;
		using const_iterator = typename std::vector<T>::const_reverse_iterator;

		void resize(std::size_t size)
		{
			m_impl.reserve(size);
		}

		stack()
		{
			resize(COVSCRIPT_STACK_PRESERVE);
		}

		explicit stack(std::size_t s)
		{
			resize(s);
		}

		~stack() = default;

		inline bool empty() const noexcept
		{
			return m_impl.empty();
		}

		inline std::size_t size() const noexcept
		{
			return m_impl.size();
		}

		inline T &top()
		{
			return m_impl.back();
		}

		inline const T &top() const
		{
			return m_impl.back();
		}

		inline T &bottom()
		{
			return m_impl.front();
		}

		inline const T &bottom() const
		{
			return m_impl.front();
		}

		inline T &index(std::size_t offset)
		{
			return m_impl.at(m_impl.size() - offset - 1);
		}

		inline const T &index(std::size_t offset) const
		{
			return m_impl.at(m_impl.size() - offset - 1);
		}

		inline T &operator[](std::size_t offset)
		{
			return m_impl[m_impl.size() - offset - 1];
		}

		inline const T &operator[](std::size_t offset) const
		{
			return m_impl[m_impl.size() - offset - 1];
		}

		template<typename...ArgsT>
		inline void push(ArgsT &&...args)
		{
			m_impl.emplace_back(std::forward<ArgsT>(args)...);
		}

		inline T pop()
		{
			T data(m_impl.back());
			m_impl.pop_back();
			return std::move(data);
		}

		inline void pop_no_return()
		{
			m_impl.pop_back();
		}

		inline void clear()
		{
			m_impl.clear();
		}

		iterator begin() noexcept
		{
			return m_impl.rbegin();
		}

		const_iterator begin() const noexcept
		{
			return m_impl.rbegin();
		}

		iterator end() noexcept
		{
			return m_impl.rend();
		}

		const_iterator end() const noexcept
		{
			return m_impl.rend();
		}
	};
}