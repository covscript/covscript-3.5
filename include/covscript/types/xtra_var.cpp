// This file is extension for variable.hpp
#pragma once
#include <covscript/types/variable.hpp>

namespace cs_impl {
	template <>
	cs::integer_t to_integer<cs::numeric_t>(const cs::numeric_t &val)
	{
		return val.as_integer();
	}

	template <>
	cs::byte_string_borrower to_string<cs::numeric_t>(const cs::numeric_t &val)
	{
		return val.to_string();
	}

	template <>
	cs::byte_string_borrower to_string<cs::byte_string_t>(const cs::byte_string_t &str)
	{
		return cs::byte_string_view(str);
	}

	template <>
	cs::byte_string_borrower to_string<cs::unicode_string_t>(const cs::unicode_string_t &str)
	{
		return cs::unicode::unicode_to_byte(str);
	}

	template <>
	cs::byte_string_borrower to_string<bool>(const bool &v)
	{
		if (v)
			return "true";
		else
			return "false";
	}

	template <int N>
	struct var_storage<char[N]>
	{
		using type = std::string;
	};

	template <>
	struct var_storage<std::type_info>
	{
		using type = std::type_index;
	};
}

// std::ostream &operator<<(std::ostream &, const cs::var &);

namespace std {
	template <>
	struct hash<cs::var>
	{
		std::size_t operator()(const cs::var &val) const
		{
			return val.hash();
		}
	};
}

namespace cs_impl::operators {
	template <typename var, typename T>
	static var add(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support + operator.");
	}

	template <typename var, typename T>
	static var sub(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support - operator.");
	}

	template <typename var, typename T>
	static var mul(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support * operator.");
	}

	template <typename var, typename T>
	static var div(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support / operator.");
	}

	template <typename var, typename T>
	static var mod(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support \% operator.");
	}

	template <typename var, typename T>
	static var pow(const T &lhs, const var &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support ^ operator.");
	}

	template <typename var, typename T>
	static var minus(const T &val)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support -val operator.");
	}

	template <typename var_borrower, typename T>
	static var_borrower escape(T &val)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support *val operator or have non-proper access privilege.");
	}

	template <typename var_borrower, typename T>
	static var_borrower escape(const T &val)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support *val operator.");
	}

	template <typename T>
	static void selfinc(T &val)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support ++ operator.");
	}

	template <typename T>
	static void selfdec(T &val)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support -- operator.");
	}

	template <typename T>
	static cs::bool_t compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

	template <typename T>
	static cs::bool_t abocmp(const T &lhs, const T &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support > operator.");
	}

	template <typename T>
	static cs::bool_t undcmp(const T &lhs, const T &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support < operator.");
	}

	template <typename T>
	static cs::bool_t aepcmp(const T &lhs, const T &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support >= operator.");
	}

	template <typename T>
	static cs::bool_t ueqcmp(const T &lhs, const T &rhs)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support <= operator.");
	}

	template <typename var_borrower, typename var, typename T>
	static var_borrower index(T &data, const var &index)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data[index] operator or have non-proper access privilege.");
	}

	template <typename var_borrower, typename var, typename T>
	static var_borrower index(const T &data, const var &index)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data[index] operator.");
	}

	template <typename var_borrower, typename T>
	static var_borrower access(T &data, const cs::byte_string_view &meber)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data.member operator or have non-proper access privilege.");
	}

	template <typename var_borrower, typename T>
	static var_borrower access(const T &data, const cs::byte_string_view &meber)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data.member operator.");
	}

	template <typename var_borrower, typename T>
	static var_borrower arrow(T &data, const cs::byte_string_view &meber)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data->member operator or have non-proper access privilege.");
	}

	template <typename var_borrower, typename T>
	static var_borrower arrow(const T &data, const cs::byte_string_view &meber)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support data->member operator.");
	}

	template <typename var_borrower, typename T>
	static var_borrower call(const T &func, cs::fwd_array &args)
	{
		throw cs::lang_error(cs::byte_string_t("Type ") + get_name_of_type<T>().data() + " does not support func(...) operator.");
	}
}

template <std::size_t align_size, template <typename> class allocator_t>
template <typename T>
cs::basic_var_borrower<align_size, allocator_t> cs::basic_var<align_size, allocator_t>::call_operator(
    cs_impl::operators::type op, bool is_const, const cs::basic_var<align_size, allocator_t> *lhs, void *rhs)
{
	using borrower_t = basic_var_borrower<align_size, allocator_t>;
	switch (op) {
	case operators_type::add:
		return cs_impl::operators::add<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::sub:
		return cs_impl::operators::sub<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::mul:
		return cs_impl::operators::mul<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::div:
		return cs_impl::operators::div<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::mod:
		return cs_impl::operators::mod<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::pow:
		return cs_impl::operators::pow<basic_var>(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->const_val<T>());
	case operators_type::minus:
		return cs_impl::operators::minus<basic_var>(lhs->template unchecked_get<T>());
	case operators_type::escape:
		return cs_impl::operators::escape<borrower_t>((is_const ? lhs : const_cast<basic_var *>(lhs))->template unchecked_get<T>());
	case operators_type::selfinc:
		if (!is_const) {
			cs_impl::operators::selfinc(const_cast<basic_var *>(lhs)->template unchecked_get<T>());
			break;
		}
		else
			throw lang_error("Operator ++ requires non-const access to variable.");
	case operators_type::selfdec:
		if (!is_const) {
			cs_impl::operators::selfdec(const_cast<basic_var *>(lhs)->template unchecked_get<T>());
			break;
		}
		else
			throw lang_error("Operator -- requires non-const access to variable.");
	// Special operators, type check finished outside.
	case operators_type::compare:
		return cs_impl::operators::compare(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
	case operators_type::abocmp:
		return cs_impl::operators::abocmp(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
	case operators_type::undcmp:
		return cs_impl::operators::undcmp(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
	case operators_type::aepcmp:
		return cs_impl::operators::aepcmp(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
	case operators_type::ueqcmp:
		return cs_impl::operators::ueqcmp(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
	case operators_type::index:
		return cs_impl::operators::index<borrower_t, basic_var>((is_const ? lhs : const_cast<basic_var *>(lhs))->template unchecked_get<T>(),
		        *static_cast<const basic_var *>(rhs));
	case operators_type::access:
		return cs_impl::operators::access<borrower_t>((is_const ? lhs : const_cast<basic_var *>(lhs))->template unchecked_get<T>(),
		        static_cast<const basic_var *>(rhs)->const_val<byte_string_t>());
	case operators_type::arrow:
		return cs_impl::operators::arrow<borrower_t>((is_const ? lhs : const_cast<basic_var *>(lhs))->template unchecked_get<T>(),
		        static_cast<const basic_var *>(rhs)->const_val<byte_string_t>());
	case operators_type::call:
		return cs_impl::operators::call<borrower_t>((lhs)->template unchecked_get<T>(), *static_cast<fwd_array *>(rhs));
	}
	return basic_var_borrower<align_size, allocator_t>();
}