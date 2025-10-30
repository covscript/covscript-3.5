#include <covscript/types/string.hpp>
#include <covscript/types/numeric.hpp>
#include <covscript/types/exception.hpp>
#include <utfcpp/utf8.h>

namespace csvm::unicode
{
	bool is_valid(byte_string_view str) noexcept
	{
		return utf8::is_valid(str);
	}

	uchar_t next(byte_string_t::const_iterator &it)
	{
		try
		{
			return utf8::unchecked::next(it);
		}
		catch (const utf8::exception &utfcpp_ex)
		{
			throw lang_error(utfcpp_ex.what());
		}
	}

	unicode_string_t byte_to_unicode(byte_string_view str)
	{
		try
		{
			unicode_string_t ustr;
			utf8::utf8to32(str.begin(), str.end(), std::back_inserter(ustr));
			return ustr;
		}
		catch (const utf8::exception &utfcpp_ex)
		{
			throw lang_error(utfcpp_ex.what());
		}
	}

	byte_string_t unicode_to_byte(unicode_string_view ustr) noexcept
	{
		byte_string_t str;
		utf8::unchecked::utf32to8(ustr.begin(), ustr.end(), std::back_inserter(str));
		return str;
	}
} // namespace csvm::unicode

namespace csvm
{
	byte_string_t numeric_t::to_string() const
	{
		if (type)
			return cs::to_string(data._int);
		else
			return cs::to_string(data._num);
	}
} // namespace csvm