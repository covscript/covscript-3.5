#pragma once
#include <covscript/types/string.hpp>
#include <covscript/types/exception.hpp>

namespace cs_impl
{
	// Name Demangle
	cs::byte_string_t cxx_demangle(const char *);

	template <typename T>
	static cs::byte_string_view get_name_of_type()
	{
		static cs::byte_string_t name = cxx_demangle(typeid(T).name());
		return name;
	}

	// Type support auto-detection(SFINAE)
	template <typename...>
	using void_t = void;

	// Compare
	template <typename _Tp>
	class compare_helper
	{
		template <typename T, typename X = bool>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(std::declval<T>() == std::declval<T>())> *)
		{
			return true;
		}

	   public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct compare_if;

	template <typename T>
	struct compare_if<T, true>
	{
		static bool compare(const T &a, const T &b)
		{
			return a == b;
		}
	};

	template <typename T>
	struct compare_if<T, false>
	{
		static bool compare(const T &a, const T &b)
		{
			return &a == &b;
		}
	};

	// To String
	template <typename _Tp>
	class to_string_helper
	{
		template <typename T, typename X>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(cs::to_string(std::declval<T>()))> *)
		{
			return true;
		}

	   public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct to_string_if;

	template <typename T>
	struct to_string_if<T, true>
	{
		static cs::byte_string_borrower to_string(const T &val)
		{
			return cs::to_string(val);
		}
	};

	// To Integer
	template <typename, bool>
	struct to_integer_if;

	template <typename T>
	struct to_integer_if<T, true>
	{
		static cs::integer_t to_integer(const T &val)
		{
			return static_cast<cs::integer_t>(val);
		}
	};

	template <typename T>
	struct to_integer_if<T, false>
	{
		static cs::integer_t to_integer(const T &)
		{
			throw cs::lang_error(cs::byte_string_t("Target type ") +
			                     get_name_of_type<T>().data() + " cannot convert to integer.");
		}
	};

	// Hash
	template <typename T, typename = void>
	struct hash_helper
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct hash_helper<T, void_t<decltype(std::hash<T>{}(std::declval<T>()))>>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	struct hash_gen_base
	{
		static std::size_t base_code;
	};

	template <typename T>
	std::size_t hash_gen_base<T>::base_code = typeid(T).hash_code();

	template <typename, typename, bool>
	struct hash_if;

	template <typename T, typename X>
	struct hash_if<T, X, true>
	{
		static std::size_t hash(const X &val)
		{
			static std::hash<T> gen;
			return gen(static_cast<const T>(val)) + hash_gen_base<X>::base_code;
		}
	};

	template <typename T>
	struct hash_if<T, T, true>
	{
		static std::size_t hash(const T &val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};

	template <typename T, typename X>
	struct hash_if<T, X, false>
	{
		static std::size_t hash(const X &val)
		{
			throw cs::lang_error(cs::byte_string_t("Target type ") +
			                     get_name_of_type<T>().data() + " unhashable.");
		}
	};

	template <typename, bool>
	struct hash_enum_resolver;

	template <typename T>
	struct hash_enum_resolver<T, true>
	{
		using type = hash_if<std::size_t, T, true>;
	};

	template <typename T>
	struct hash_enum_resolver<T, false>
	{
		using type = hash_if<T, T, hash_helper<T>::value>;
	};

	/*
	 * Type support
	 * You can specialize template functions to support type-related functions.
	 * But if you do not have specialization, CovScript can also automatically detect related functions.
	 */
	template <typename T>
	static cs::byte_string_borrower to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template <typename T>
	static cs::integer_t to_integer(const T &val)
	{
		return to_integer_if<T, std::is_convertible<T, cs::integer_t>::value>::to_integer(val);
	}

	template <typename T>
	static std::size_t hash(const T &val)
	{
		using type = typename hash_enum_resolver<T, std::is_enum<T>::value>::type;
		return type::hash(val);
	}

	template <typename T>
	struct to_string_if<T, false>
	{
		static cs::byte_string_borrower to_string(const T &)
		{
			return "[" + cs::byte_string_t(get_name_of_type<T>()) + "]";
		}
	};

	// template<typename T>
	// static cs::namespace_t &get_ext()
	// {
	// 	throw cs::runtime_error(std::string("Target type \"") + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) +
	// 	                        "\" doesn't have extension field.");
	// }

	template <typename _Target>
	struct type_conversion_cs
	{
		using source_type = _Target;
		using _not_specialized = void;
	};

	template <typename _Source>
	struct type_conversion_cpp
	{
		using target_type = _Source;
		using _not_specialized = void;
	};

	template <typename _From, typename _To>
	struct type_convertor
	{
		using _not_specialized = void;
		template <typename T>
		static inline _To convert(T &&val) noexcept
		{
			return std::move(static_cast<_To>(std::forward<T>(val)));
		}
	};

	template <typename T>
	struct type_convertor<T, T>
	{
		template <typename X>
		static inline X &&convert(X &&val) noexcept
		{
			return std::forward<X>(val);
		}
	};

	template <typename T>
	struct type_convertor<T, void>
	{
		template <typename X>
		static inline void convert(X &&) noexcept
		{
		}
	};

	template <typename T>
	struct var_storage
	{
		using type = T;
	};

	template <typename T>
	using var_storage_t = typename var_storage<std::decay_t<T>>::type;

	namespace operators
	{
		enum class type
		{
			add,     // lhs + rhs
			sub,     // lhs - rhs
			mul,     // lhs * rhs
			div,     // lhs / rhs
			mod,     // lhs % rhs
			pow,     // lhs ^ rhs
			minus,   // -val
			escape,  // *val
			selfinc, // ++val
			selfdec, // --val
			compare, // lhs == rhs
			abocmp,  // lhs > rhs
			undcmp,  // lhs < rhs
			aepcmp,  // lhs >= rhs
			ueqcmp,  // lhs <= rhs
			index,   // data[index]
			access,  // data.member
			arrow,   // data->member
			call     // func(args)
		};
	}
} // namespace cs_impl