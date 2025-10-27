#pragma once
#include <string>

namespace cs {
	using char_t = char;
	using uchar_t = char32_t;
	using byte_string_t = std::string;
	using byte_string_view = std::string_view;
	using unicode_string_t = std::u32string;
	using unicode_string_view = std::u32string_view;

	namespace unicode {
		bool is_valid(byte_string_view) noexcept;

		uchar_t next(byte_string_t::const_iterator &);

		unicode_string_t byte_to_unicode(byte_string_view);

		byte_string_t unicode_to_byte(unicode_string_view) noexcept;
	}
}