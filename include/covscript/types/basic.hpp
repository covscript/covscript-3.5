#pragma once
#include <cstdlib>
#include <string>
#include <vector>

#ifndef COVSCRIPT_STACK_PRESERVE
#define COVSCRIPT_STACK_PRESERVE 64
#endif

#ifndef COVSCRIPT_BLOCK_ALLOCATOR_SIZE
#define COVSCRIPT_BLOCK_ALLOCATOR_SIZE 64
#endif

namespace cs
{
	using bool_t = bool;
	using char_t = char;
	using uchar_t = char32_t;
	using float_t = long double;
	using integer_t = long long int;
	using byte_string_t = std::basic_string<char>;
	using byte_string_view = std::basic_string_view<char>;
	using unicode_string_t = std::basic_string<char32_t>;
	using unicode_string_view = std::basic_string_view<char32_t>;

	template <typename T>
	class stack final
	{
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

		inline T &index(std::size_t index)
		{
			return m_impl.at(index);
		}

		inline const T &index(std::size_t index) const
		{
			return m_impl.at(index);
		}

		inline T &operator[](std::size_t index)
		{
			return m_impl[index];
		}

		inline const T &operator[](std::size_t index) const
		{
			return m_impl[index];
		}

		template <typename... ArgsT>
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

	template <typename T,
	          std::size_t block_size = COVSCRIPT_BLOCK_ALLOCATOR_SIZE,
	          template <typename> class allocator_t = std::allocator>
	class allocator_type
	{
		allocator_t<T> m_alloc;
		T *m_pool[block_size];
		std::size_t m_offset;

	   public:
		allocator_type() : m_offset(0)
		{
			for (std::size_t i = 0; i < block_size / 2; ++i)
				m_pool[i] = m_alloc.allocate(1);
			m_offset = block_size / 2;
		}

		~allocator_type()
		{
			for (std::size_t i = 0; i < m_offset; ++i)
				m_alloc.deallocate(m_pool[i], 1);
		}

		allocator_type(const allocator_type &) = delete;
		allocator_type &operator=(const allocator_type &) = delete;

		inline T *allocate(std::size_t n)
		{
			if (n == 1 && m_offset > 0)
				return m_pool[--m_offset];
			else
				return m_alloc.allocate(n);
		}

		inline void deallocate(T *ptr, std::size_t n)
		{
			if (n == 1 && m_offset < block_size)
				m_pool[m_offset++] = ptr;
			else
				m_alloc.deallocate(ptr, n);
		}
	};

#ifdef COVSCRIPT_COMPATIBILITY_MODE
	template <typename T>
	using default_allocator = std::allocator<T>;
#else
	template <typename T>
	using default_allocator = allocator_type<T>;
#endif
} // namespace cs