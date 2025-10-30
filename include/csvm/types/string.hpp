#pragma once
#include <csvm/types/basic.hpp>
#include <memory>
#include <string>

namespace csvm
{
	using std::to_string;

	template <typename CharT,
	          template <typename> class allocator_t = default_allocator>
	class basic_string_borrower final
	{
		using stl_string = std::basic_string<CharT>;
		using stl_string_view = std::basic_string_view<CharT>;
		using allocator_type = allocator_t<stl_string>;

		static allocator_type &get_allocator()
		{
			static allocator_type allocator;
			return allocator;
		}

		void *m_data = nullptr;
		bool m_own = false;

		void destroy()
		{
			if (m_own && m_data)
			{
				stl_string *p = static_cast<stl_string *>(m_data);
				p->~basic_string();
				get_allocator().deallocate(p, 1);
			}
			m_data = nullptr;
			m_own = false;
		}

	   public:
		basic_string_borrower() noexcept = default;

		basic_string_borrower(const CharT *str) noexcept : m_data(const_cast<CharT *>(str)), m_own(false) {}

		basic_string_borrower(stl_string_view view) noexcept : m_data(const_cast<CharT *>(view.data())), m_own(false) {}

		basic_string_borrower(stl_string str) : m_own(true)
		{
			stl_string *p = get_allocator().allocate(1);
			::new (p) stl_string(std::move(str));
			m_data = p;
		}

		basic_string_borrower(const basic_string_borrower &other) : m_own(other.m_own)
		{
			if (other.m_own)
			{
				stl_string *p = get_allocator().allocate(1);
				::new (p) stl_string(*static_cast<stl_string *>(other.m_data));
				m_data = p;
			}
			else
				m_data = other.m_data;
		}

		basic_string_borrower(basic_string_borrower &&other) noexcept
		    : m_data(other.m_data), m_own(other.m_own)
		{
			other.m_data = nullptr;
			other.m_own = false;
		}

		basic_string_borrower &operator=(const basic_string_borrower &other)
		{
			if (this != &other)
			{
				destroy();
				m_own = other.m_own;
				if (other.m_own)
				{
					stl_string *p = get_allocator().allocate(1);
					::new (p) stl_string(*static_cast<stl_string *>(other.m_data));
					m_data = p;
				}
				else
					m_data = other.m_data;
			}
			return *this;
		}

		basic_string_borrower &operator=(basic_string_borrower &&other) noexcept
		{
			if (this != &other)
			{
				destroy();
				m_data = other.m_data;
				m_own = other.m_own;
				other.m_data = nullptr;
				other.m_own = false;
			}
			return *this;
		}

		~basic_string_borrower()
		{
			destroy();
		}

		stl_string_view view() const noexcept
		{
			if (m_own)
				return *static_cast<stl_string *>(m_data);
			else if (m_data)
				return stl_string_view(static_cast<CharT *>(m_data));
			else
				return stl_string_view{};
		}

		const CharT *data() const noexcept
		{
			if (m_own)
				return static_cast<stl_string *>(m_data)->data();
			else if (m_data)
				return static_cast<CharT *>(m_data);
			else
				return nullptr;
		}

		bool usable() const noexcept
		{
			return m_data != nullptr;
		}

		operator bool() const noexcept
		{
			return usable();
		}

		stl_string *access() noexcept
		{
			return m_own ? static_cast<stl_string *>(m_data) : nullptr;
		}
	};

	using byte_string_borrower = basic_string_borrower<char>;
	using unicode_string_borrower = basic_string_borrower<char32_t>;

	namespace unicode
	{
		bool is_valid(byte_string_view) noexcept;

		uchar_t next(byte_string_t::const_iterator &);

		unicode_string_t byte_to_unicode(byte_string_view);

		byte_string_t unicode_to_byte(unicode_string_view) noexcept;
	} // namespace unicode
} // namespace csvm